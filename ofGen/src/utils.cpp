#include "utils.h"
#include <fstream> // ifstream
#include <iostream> // cout
#include <regex>
#include <vector>

genConfig conf;

std::string ofPathToString(const fs::path & path) {
	try {
		return path.string();
	} catch (fs::filesystem_error & e) {
		std::cerr << "ofPathToString: error converting fs::path to string " << e.what();
	}
	return {};
}

std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
	return std::regex_replace(strIn, std::regex(from), to);
}

bool ofIsPathInPath(const fs::path & path, const fs::path & base) {
	if (path == base) {
		return true;
	}
	auto rel = fs::relative(path, base);
	// bool isP = !rel.empty() && rel.native()[0] != '.';
	// cout << "ofIsPathInPath " << path << " : " << base << " : " << isP << endl;

	return !rel.empty() && rel.native()[0] != '.';
}
// std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
// 	return std::regex_replace(strIn, std::regex(from), to);
// }
//

std::string textToString(const fs::path & file) {
	std::ifstream fileFrom(file);
	std::string contents((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
	fileFrom.close();
	return contents;
}

std::vector<std::string> textToVector(const fs::path & file) {
	std::vector<std::string> out;
	if (fs::exists(file)) {
		std::ifstream thisFile(file);
		std::string line;
		while (getline(thisFile, line)) {
			out.emplace_back(line);
		}
	}
	return out;
}

void replaceAll(std::string & str, const std::string & from, const std::string & to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

void ltrim(std::string & s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
void rtrim(std::string & s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(),
		s.end());
}

std::string ofTrim(std::string line) {
	rtrim(line);
	ltrim(line);
	// line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());
	return line;
}

std::vector<std::string> ofSplitString(const std::string & s, const std::string & delimiter) {
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

// void genConfig::open() {
// }

void genConfig::import() {
	if (fs::exists("of.yml")) {
		alert("of.yml already present", 32);
	} else {
		std::ifstream file("addons.make");
		YAML::Node node;
		node["ofpath"] = "../../..";
		if (file.is_open()) {
			std::string line;
			while (std::getline(file, line)) {
				// msg(line, 33);
				// next line trims whitespace at the end.
				line =  line.substr(0, line.find_last_not_of(" \f\n\r\t\v") + 1);
				node["addons"].push_back(line);
			}
		}
		file.close();
		// std::string saida = node.as<std::string>();

		// for (auto & t : templateNames) {
		// 	node["templates"].push_back(t);
		// }
		// node["templates"].SetMapStyle(YAML::Flow);

		// YAML::Emitter out;
		// out << YAML::Flow;
		// out << YAML::BeginSeq;
		// for (auto & t : templateNames) {
		// 	out << t;
		// }
		// out << YAML::EndSeq;
		// out << YAML::Flow;
		// out << YAML::BeginSeq << 2 << 3 << 5 << 7 << 11 << YAML::EndSeq;

		std::ofstream ofYml("of.yml");
		cout << node << endl;
		ofYml << node;
		ofYml.close();
		cout << endl;
		alert("ok, of.yml created from addons.make", 32);
	}
}

bool genConfig::loadYML() {
	fs::path configFile { "of.yml" };
	if (!fs::exists(configFile)) {
		alert("no of.yml present. use `ofgen import` to create one from addons.make", 32);
		return false;
		// alert("missing of.yml file ", 31);
	} else {
		// config = YAML::LoadFile(configFile);
		config = YAML::LoadFile(configFile.string());
		if (config["ofpath"]) { // use ofpath only if the key exists.
			auto ofPathYML = config["ofpath"];
			ofPath = ofPathYML.as<std::string>();
		}

		conf.addonsNames = nodeToStrings("addons");

		auto templateNames = nodeToStrings("templates");
		if (templateNames.size() > 0) {
			conf.templateNames = templateNames;
		} else {
		    // FIXME: this is an error in linux64 (no template with this name)
			// better idea is having a lookup table with platforms and default templates.
			// and apply directly to ofgen import
			alert("No templates found, ofgen will deduce from platform", 95);
			conf.templateNames.emplace_back(getPlatformString());
		}

		cout << endl;

		if (!conf.templateNames.size()) {
			alert("No templates selected, exiting", 95);
			exit(0);
		}

		alert("Templates ");
		for (auto & t : conf.templateNames) {
			cout << t << endl;
		}

		// FIXME: no lugar disso fazer mesmo um map<string, vector<string>> pra parsear todos de uma vez.
		conf.frameworks = nodeToStrings("frameworks");
		conf.defines = nodeToStrings("defines");

		// FIXME: Maybe rename to additional Sources?
		conf.additionalSources = nodeToPaths("sources");
		if (conf.additionalSources.size()) {
			alert("Additional Source Directories ");
			for (auto & a : conf.additionalSources) {
				cout << a << endl;
			}
		}

		if (conf.addonsNames.size()) {
			alert("Addons");
			for (auto & a : conf.addonsNames) {
				cout << a << endl;
			}
		}
		cout << endl;
	}

	return true;
}

std::vector<std::string> genConfig::nodeToStrings(const std::string & index) {
	std::vector<std::string> out;
	if (config[index]) {
		auto items = config[index];
		for (std::size_t i = 0; i < items.size(); i++) {
			out.emplace_back(items[i].as<std::string>());
		}
	}
	return out;
}

std::vector<fs::path> genConfig::nodeToPaths(const std::string & index) {
	std::vector<fs::path> out;
	if (config[index]) {
		auto items = config[index];
		for (std::size_t i = 0; i < items.size(); i++) {
			out.emplace_back(items[i].as<std::string>());
		}
	}
	return out;
}
