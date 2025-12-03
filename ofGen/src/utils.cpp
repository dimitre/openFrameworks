#include "utils.h"
#include "addons.h"
#include "ofTemplateVSCode.h"
#include "ofTemplateChalet.h"
#include "templates.h"

#include <fstream> // ifstream
#include <iostream> // cout
#include <regex>
#include <vector>

genConfig conf;

void ofProject::build() {
	divider();
	alert("ofProject::build", 92);
	// std::cout << "addons.size " << addons.size() << std::endl;
	// std::cout << "templates.size " << templates.size() << std::endl;

	// each template for specific project
	for (auto & t : templates) {
		t->load();
		t->info();

		alert("	Building template " + t->name, 95);
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
		if (!isValidProjectLocation()) {
			alert("⚠️  not a valid ofWorks project folder. exiting", 91);
			// alert("⚠️  Not an ofWorks project folder, no action taken", 94);

		} else {
			std::ifstream file("addons.make");
			YAML::Node node;
			node["ofpath"] = "../../..";
			if (file.is_open()) {
				std::string line;
				while (std::getline(file, line)) {
					// msg(line, 33);
					// next line trims whitespace at the end.
					line = line.substr(0, line.find_last_not_of(" \f\n\r\t\v") + 1);
					node["addons"].push_back(line);
				}
			}
			file.close();

			std::ofstream ofYml("of.yml");
			cout << node << endl;
			ofYml << node;
			ofYml.close();
			cout << endl;
			alert("ok, of.yml created from addons.make", 32);
		}
	}
}

bool genConfig::loadYML() {
	fs::path configFile { "of.yml" };
	if (!fs::exists(configFile)) {
		// alert("no of.yml present. use `ofgen import` to create one from addons.make", 32);
		return false;
		// alert("missing of.yml file ", 31);
	} else {
		// config = YAML::LoadFile(configFile);
		ofYaml = YAML::LoadFile(configFile.string());
		if (ofYaml["ofpath"]) { // use ofpath only if the key exists.
			auto ofPathYML = ofYaml["ofpath"];
			ofPath = ofPathYML.as<std::string>();
		}

		conf.addonsNames = nodeToStrings("addons");

		if (ofYaml["addonsSources"]) {
			for (auto c : ofYaml["addonsSources"]) {
				auto name { c["name"].as<std::string>() };
				auto repo { c["repository"].as<std::string>() };
				if (fs::exists(ofPath / "addons" / name)) {
					// FIXME: Check if it exists
					// Transform to a unique set...
					//
					if (std::find(conf.addonsNames.begin(), conf.addonsNames.end(), name) == conf.addonsNames.end()) {
						conf.addonsNames.emplace_back(name);
					} else {
						cout << "addon already added" << endl;
					}
					cout << name << endl;
					cout << repo << endl;
					cout << "------" << endl;
				}
			}
		}

		if (empty(conf.templateNames)) {
			auto templateNames = nodeToStrings("templates");
			if (templateNames.size() > 0) {
				conf.templateNames = templateNames;
			} else {
				// FIXME: this is an error in linux64 (no template with this name)
				// better idea is having a lookup table with platforms and default templates.
				// and apply directly to ofgen import
			}
		} else {
			alert("using templates from command line parameter");
		}
		cout << endl;

		if (!conf.templateNames.size()) {
			alert("No templates selected, exiting", 95);
			exit(0);
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
			// for (auto & a : conf.addonsNames) {
			// 	cout << a << endl;
			// }
			cout << joinStrings(conf.addonsNames, ", ") << endl;
		}

		if (conf.platforms.size()) {
			alert("Platforms");
			cout << joinStrings(conf.platforms, ", ") << endl;
		} else {
			alert("No Platforms Yet");
		}
		cout << endl;
	}

	return true;
}

std::vector<std::string> genConfig::nodeToStrings(const std::string & index) {
	std::vector<std::string> out;
	if (ofYaml[index]) {
		auto items = ofYaml[index];
		for (std::size_t i = 0; i < items.size(); i++) {
			out.emplace_back(items[i].as<std::string>());
		}
	}
	return out;
}

std::vector<fs::path> genConfig::nodeToPaths(const std::string & index) {
	std::vector<fs::path> out;
	if (ofYaml[index]) {
		auto items = ofYaml[index];
		for (std::size_t i = 0; i < items.size(); i++) {
			out.emplace_back(items[i].as<std::string>());
		}
	}
	return out;
}

bool genConfig::buildProject() {

	// ofProject project;
	//
	if (!isValidProjectLocation()) {

		// MV desc to inside isValidProjectLocation check
		alert("⚠️  not a valid ofWorks project folder, no action taken", 91);
		alert("no src folder found, no of.yml file or addons.make found and no OF installed in default path ../../..");
		alert("use ```ofgen help``` to see more options");

		// ◢██
		// build = false;
		return false;
		// conf.help();
	}

	bool hasYml = loadYML();
	if (hasYml) {
		alert("of.yml found, building from there", 95);
	} else {

		if (!isValidOfPath()) {
			alert("OF not found in default path " + ofPath.string());
			help();
			return false;
		} else {
			alert("of path OK, proceeding");

			if (!fs::exists("bin")) {
				alert("bin folder not found, creating");
				fs::create_directory("bin");
			}
		}

		alert("building from addons.make", 95);
		fs::path addonsListFile { projectPath / "addons.make" };
		if (fs::exists(addonsListFile)) {
			addonsNames = textToVector(addonsListFile);
		} else {
			alert("no addons.make found", 95);
		}

		alert("No templates found (of.yml or parameters), ofgen will deduce from platform", 95);

		std::map<std::string, std::vector<std::string>> platformTemplates {
			{ "macos", { "chalet", "zed", "macos" } },
			{ "vs", { "chalet", "zed" } },
			{ "linux64", { "chalet", "zed" } },
			{ "linuxaarch64", { "chalet", "zed" } },
			// { "vs", { "visualstudio" } },
			// { "msys2", { "make", "vscode" } },
			// { "linux64", { "make", "vscode" } },
		};

		std::string platform { getPlatformString() };
		cout << "platform is " << platform << endl;
		if (!empty(platform)) {
			if (empty(templateNames)) {
				templateNames = platformTemplates[platform];
			} else {
			}
		}

		alert("Templates ");
		cout << joinStrings(templateNames, ", ") << endl;
	}

	// scanFolder()
	// create templates, add to project
	for (auto & t : templateNames) {
		if (t == "chalet") {
			templates.emplace_back(new ofTemplateChalet());
			project.templates.emplace_back(templates.back());
		} else if (t == "zed") {
			templates.emplace_back(new ofTemplateZed());
			project.templates.emplace_back(templates.back());
		} else if (t == "macos") {
			templates.emplace_back(new ofTemplateMacos());
			project.templates.emplace_back(templates.back());
		} else if (t == "vscode") {
			templates.emplace_back(new ofTemplateVSCode());
			project.templates.emplace_back(templates.back());
		}
		// else if (t == "make") {
		// 	templates.emplace_back(new ofTemplateMake());
		// 	project.templates.emplace_back(templates.back());
		// }

		// else if (t == "visualstudio") {
		// 	templates.emplace_back(new ofTemplateVisualStudio());
		// 	project.templates.emplace_back(templates.back());
		// }

		else {
			alert("invalid template name : " + t + ", exiting", 95);
			return false;
			// std::exit(1);
		}

		if (empty(openCommand) && !empty(templates.back()->openCommand)) {
			openCommand = templates.back()->openCommand;
		}

		if (empty(buildCommand) && !empty(templates.back()->buildCommand)) {
			buildCommand = templates.back()->buildCommand;
		}

		if (empty(runCommand) && !empty(templates.back()->runCommand)) {
			runCommand = templates.back()->runCommand;
		}
	}

	// load templates, show info of each template
	// for (auto & t : templates) {
	// 	// cout << t->name << " : " << t->path << endl;
	// 	t->load();
	// 	t->info();
	// }

	// now parse project addons, or yml

	if (!fs::exists("./src")) {
		// FIXME: check if template is ios and copy mm files accordingly. if not copy src files from templates.
		fs::path from { ofPath / "scripts" / "templates" / "src" };
		fs::path to { "./src" };
		alert(from.string(), 95);
		alert(fs::current_path().string(), 95);
		try {
			fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::update_existing);
		} catch (fs::filesystem_error & e) {
			std::cerr << "error copying template file " << from << " : " << to << std::endl;
			return false;
		}
	}
	// exit(1);

	// DELICATE. treating projects as an addon.
	// it works well. not delicate anymore.
	// src will always exist because we copy them if not.
	// if (fs::exists("./src"))
	{
		{
			addons.push_back(new ofAddon());
			ofAddon * addon = addons.back();
			addon->isProject = true;
			addon->name = "ProjectSourceFiles_" + projectName;
			addon->path = "";

			for (auto & f : frameworks) {
				addon->filesMap["frameworks"].emplace_back(f);
			}

			// addon->showFiles();
			// addon->info();
			for (auto & path : additionalSources) {
				addon->filesMap["includes"].emplace_back(path);
			}
			addon->load();
			// addons.emplace_back(addon);
			project.addons.emplace_back(addons.back());
		}

		// TODO: Add here additional sources
		// for (auto & a : additionalSources) {
		// 	alert(">> Additional Sources Folder: " + a.string(), 95);
		// 	addons.push_back(new ofAddon());
		// 	ofAddon * addon = addons.back();
		// 	addon->isProject = true;
		// 	addon->name = "AdditionalSource_" + projectName;
		// 	addon->path = a;
		// 	addon->isProject = true;

		// 	scanFolder(a, addon->filesMap, true);
		// 	addon->load();
		// 	project.addons.emplace_back(addons.back());
		// }
	}
	// else {
	// 	alert("NO SRC FILE FOUND IN PROJECT", 95);
	// 	std::exit(1);
	// }

	// fs::path addonsListFile { projectPath / "addons.make" };
	// if (fs::exists(addonsListFile)) {
	// vector<std::string> addonsList { textToVector(addonsListFile) };
	// vector<std::string> addonsList = { "ofxMidi" }; //ofxMidi ofxOpenCv

	for (auto & l : addonsNames) {
		if (l != "") {
			addons.push_back(new ofAddon());
			ofAddon * addon = addons.back();

			// ofAddon addon;
			addon->name = l;
			// check if local addon exists, if not check in of addons folder.
			if (fs::exists(projectPath / l)) {
				addon->path = projectPath / l;
			} else {
				if (fs::exists(ofPath / "addons" / l)) {
					addon->path = ofPath / "addons" / l;
				}
			}

			if (std::empty(addon->path)) {
				continue;
			}

			addon->load();
			// addons.emplace_back(addon);
			project.addons.emplace_back(addons.back());
		}
	}
	// }

	// pass files to projects.
	project.build();

	return true;
}
