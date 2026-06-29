#include "utils.h"
#include "addons.h"
#include "ofTemplate.h"

#include <algorithm> // find_if, remove_if
#include <fstream> // ifstream
#include <iostream> // cout
#include <vector>

#include <fmt/format.h>
// genConfig conf;

void ofProject::build() {
	divider();
	alert("ofProject::build", 92);
	// std::cout << "addons.size " << addons.size() << std::endl;
	// std::cout << "templates.size " << templates.size() << std::endl;

	// each template for specific project
	for (auto & t : templates) {
		// alert("	Building template " + t->name, 95);
		t->load();
		t->info();

		// each addon for specific project
		for (auto & a : addons) {
			// alert("	ofProject::addAddon " + t->name + " : " + a->name, 34);
			t->addAddon(a);
		}
		t->save();
		t->build();
	}
}

void ofProject::cleanTemplates() {
	divider();
	alert("ofProject::cleanTemplates", 92);
	for (auto & t : templates) {
		t->load();
		t->cleanTemplateFiles();
	}
}

std::string ofPathToString(const fs::path & path) {
	try {
		return path.string();
	} catch (fs::filesystem_error & e) {
		std::cerr << "ofPathToString: error converting fs::path to string " << e.what();
	}
	return {};
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

bool removePath(const fs::path & path) {
	std::error_code ec;
	auto count = fs::remove_all(path, ec);
	if (ec) {
		alert("could not remove " + path.string() + " : " + ec.message(), 91);
		return false;
	}
	if (count > 0) {
		alert("removed " + path.string(), 90);
	}
	return count > 0;
}

void removePathsByExtension(const fs::path & dir, const std::string & ext) {
	std::error_code ec;
	if (!fs::exists(dir, ec)) return;
	for (const auto & entry : fs::directory_iterator(dir, ec)) {
		if (entry.path().extension() == ext) {
			removePath(entry.path());
		}
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
