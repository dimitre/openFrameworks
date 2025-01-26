#include "addons.h"
#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;
#include "utils.h"
#include <glob.h>

bool ofIsPathInPath(const fs::path & path, const fs::path & base) {
	auto rel = fs::relative(path, base);
	return !rel.empty() && rel.native()[0] != '.';
}

void ofAddon::load() {
	loadAddonConfig();
	loadFiles();
	relative();
	refine();
	// showFiles();
}

void ofAddon::relative() {
	alert("relative :: addon " + name, 91);
	for (auto & f : filesMap) {
		for (auto & s : f.second) {
			s = fs::relative(s, path);
		}
	}
}

void ofAddon::refine() {
	alert("refine :: addon " + name, 91);

	for (const auto & f : filesMap) {
		for (const auto & s : f.second) {
			for (const auto & e : exclusionsMap[f.first]) {
				if (!ofIsPathInPath(s, e)) {
					filteredMap[f.first].emplace_back(s);
					// std::cout << s << std::endl;
					// alert("added " + s.string(), 92);
				} else {

					alert("	excluded " + s.string(), 93);
					alert("	exclusion=" + e.string() + ", section=" + f.first, 33);
					// alert (, 93);
				}
			}
		}
	}

	// if (addonProperties.contains("ADDON_SOURCES_EXCLUDE")) {
	// 	alert("ADDON_SOURCES_EXCLUDE not empty :: ");
	// 	for (auto & a : addonProperties["ADDON_SOURCES_EXCLUDE"]) {
	// 		string value = stringReplace(a, "/%", "");
	// 		alert(value, 92);
	// 		exclusionsMap["sources"].emplace_back(value);
	// 	}
	// 	for (auto & e : addonProperties["ADDON_SOURCES_EXCLUDE"]) {
	// 		alert(e);
	// 	}

	// } else {
	// 	alert("ADDON_SOURCES_EXCLUDE empty");
	// }
}

void ofAddon::showFiles() {
	alert("showFiles :: addon " + name, 91);
	for (auto & f : filesMap) {
		alert(f.first + ":", 31);
		for (auto & s : f.second) {
			std::cout << s << std::endl;
		}
	}
}

void ofAddon::loadFiles() {
	alert("loadFiles :: addon " + name, 91);

	scanFolder(path / "src", filesMap, true);

	// get addon libs, it can be none, one or multiple
	fs::path folderLibs { path / "libs" };

	if (fs::exists(folderLibs)) {
		alert("	libsPath found " + folderLibs.string(), 32);

		bool hasPlatformFolder = false;
		// this iterate each lib inside one addon, ex: ofxSvg has svgtiny and libxml2
		for (auto const & lib : fs::directory_iterator { folderLibs }) {
			auto f = lib.path();
			if (!fs::is_directory(f)) {
				continue;
			}
			//
			alert("		" + f.string(), 95);
			auto includeFolder { f / "include" };
			if (fs::exists(includeFolder)) {
				scanFolder(includeFolder, filesMap, true);
			}

			if (fs::exists(f / "lib")) {
				for (const std::string p : conf.platforms) {

					fs::path folder { f / "lib" / p };
					if (!fs::exists(folder)) {
						alert("		folder don't exist " + folder.string(), 96);
						continue;
					} else {
						hasPlatformFolder = true;
						scanFolder(folder, filesMap, true);
						alert("		folder yes exist " + folder.string(), 94);
					}
				}
			}
		}

		if (!hasPlatformFolder) {
			alert("		don't have platform folder, will scan everything " + folderLibs.string(), 92);
			scanFolder(folderLibs, filesMap, true);
		}
	}
}

void ofAddon::loadAddonConfig() {
	alert("loadAddonConfig :: addon " + name, 91);

	fs::path addonConfig { path / "addon_config.mk" };
	// alert ("zed " + fileName.string(), 91);
	if (!fs::exists(addonConfig)) {
		// alert(" addonConfig not found " + addonConfig.string(), 31);
		return;
	} else {
		alert("	addonConfig found " + addonConfig.string(), 32);
	}

	int lineNum = 0;
	for (auto & originalLine : textToVector(addonConfig)) {
		lineNum++;
		string line = originalLine;

		line = ofTrim(line);

		// discard comments and blank lines
		if (line[0] == '#' || line == "") {
			continue;
		}

		line = stringReplace(line, " \\= ", "=");
		line = stringReplace(line, "\\= ", "=");
		line = stringReplace(line, " \\=", "=");
		line = stringReplace(line, " \\+\\= ", "+=");
		line = stringReplace(line, " \\+\\=", "+=");
		line = stringReplace(line, "\\+\\= ", "+=");

		// Trim., removing whitespace
		// line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());

		if (line[line.size() - 1] == ':') {
			stringReplace(line, ":", "");
			currentParseState = line;
		}

		// if (
		//   currentParseState != "common"
		//           && currentParseState != "macos"
		//           && currentParseState != "osx"
		//           // && currentParseState != "emscripten"
		// ) {
		// continue;
		// }

		if (line.find("=") != string::npos) {
			bool addToValue = false;
			vector<string> varValue;
			bool limpa = false;
			if (line.find("+=") != string::npos) {
				addToValue = true;
				// FIXME: maybe not needed. a simple split is ok.
				varValue = splitStringOnceByLeft(line, "+=");
			} else {
				limpa = true;
				addToValue = false;
				varValue = splitStringOnceByLeft(line, "=");
			}

			// variable = ofTrim(varValue[0]);
			// value = ofTrim(varValue[1]);
			string variable = varValue[0];
			string value = varValue[1];
			if (limpa) {
				addonProperties[variable].clear();
			}
			if (value != "") {
				addonProperties[variable].emplace_back(value);
			}
		}
	}

	// for (auto & a : addonProperties) {
	// 	alert("    	" + a.first, 94);
	// 	for (auto & p : a.second) {
	// 		alert("     	  " + p, 95);
	// 	}
	// }

	const static std::map<std::string, std::string> exclusionsType {
		{ "ADDON_SOURCES_EXCLUDE", "sources" },
		{ "ADDON_INCLUDES_EXCLUDE", "includes" },
		{ "ADDON_LIBS_EXCLUDE", "libs" },
	};

	for (auto & e : exclusionsType) {
		if (addonProperties.contains(e.first)) {
			// alert(e.first + " not empty");

			for (auto & a : addonProperties[e.first]) {
				string value = stringReplace(a, "/%", "");
				// alert(value, 92);
				exclusionsMap[e.second].emplace_back(value);
			}
		} else {
			// alert(e.first + " empty");
		}
	}
	//std::map<string, vector<string> > addonProperties;
}

bool copyTemplateFile::run() {

	if (fs::exists(from)) {
		// ofLogVerbose() << "copyTemplateFile from: " << from << " to: " << to;
		alert("base::copyTemplateFile from: " + from.string() + " to: " + to.string(), 33);

		if (findReplaces.size() || appends.size()) {
			// Load file, replace contents, write to destination.
			std::ifstream fileFrom(from);
			std::string contents((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
			fileFrom.close();

			for (auto & f : findReplaces) {
				// Avoid processing empty pairs
				if (empty(f.first) && empty(f.second)) {
					continue;
				}
				replaceAll(contents, f.first, f.second);
				// ofLogVerbose() << "└─ Replacing " << f.first << " : " << f.second;
				std::cout << "└─ Replacing " << f.first << " : " << f.second << std::endl;
			}

			for (auto & a : appends) {
				// alert(a, 35);
				contents += "\n" + a;
			}

			std::ofstream fileTo(to);
			try {
				fileTo << contents;
			} catch (std::exception & e) {
				std::cerr << "Error saving to " << to << std::endl;
				std::cerr << e.what() << std::endl;
				return false;
			} catch (...) {
				std::cerr << "Error saving to " << to << std::endl;

				return false;
			}

		} else {
			// no replacements, straight copy
			if (isFolder) {
				try {
					if (!fs::exists(to)) {
						fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::update_existing);
					}
				} catch (const std::exception & e) {
					std::cerr << "Error copying template files: " << e.what() << std::endl;
					return false;
				}
			} else {
				try {
					fs::copy(from, to, fs::copy_options::update_existing);
				} catch (fs::filesystem_error & e) {
					std::cerr << "error copying template file " << from << " : " << to << std::endl;
					std::cerr << e.what() << std::endl;
					return false;
				}
			}
		}
	} else {
		return false;
	}
	return true;
}

// void scanFolder(const fs::path & path, std::map<std::string, std::vector<fs::path>> & filesMap, bool recursive) {

void scanFolder(const fs::path & path,
	std::map<std::string, std::vector<fs::path>> & filesMap,
	// std::map<std::string, std::vector<fs::path>> & exclusionsMap,
	bool recursive) {
	// it should exist and be a folder.
	if (!fs::exists(path)) return;
	if (!fs::is_directory(path)) return;

	alert("    scanFolder " + path.string(), 97);

	filesMap["includes"].emplace_back(path);

	for (auto it = fs::recursive_directory_iterator(path);
		 it != fs::recursive_directory_iterator();
		 ++it) {

		auto f = it->path();

		// avoid hidden folders like .git etc.
		if (f.filename().c_str()[0] == '.') {
			it.disable_recursion_pending();
			continue;
		}

		if (!recursive && it.depth() > 0) {
			it.disable_recursion_pending();
			continue;
		}

		// alert(f);

		auto ext = f.extension().string();
		if (fs::is_directory(f)) {
			if (ext == ".framework" || ext == ".xcframework") {
				// ADD To Frameworks List, and stop searching inside this directory
				filesMap["frameworks"].emplace_back(f);
				it.disable_recursion_pending();
				continue;
			} else {
				// ADD To includes list
				filesMap["includes"].emplace_back(f);
				// std::cout << conf.filesMap["includes"].size() << std::endl;
			}

		} else {
			if (ext == ".a" || ext == ".lib") {
				filesMap["libs"].emplace_back(f);
			} else if (ext == ".dylib" || ext == ".so" || ext == ".dll") {
				filesMap["sharedLibs"].emplace_back(f);
			} else if (ext == ".h" || ext == ".hpp" || ext == ".m" || ext == ".tcc" || ext == ".inl" || ext == ".in") {
				filesMap["headers"].emplace_back(f);
			} else if (ext == ".c" || ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".mm") {
				filesMap["sources"].emplace_back(f);
			} else {
				alert("		no desired extension " + f.string(), 94);
			}
		}
	}
}

void gatherProjectInfo() {
	alert("gatherProjectInfo", 92);
	// Add project files. TODO: additional source folders

	conf.ofTemplates.emplace_back(new ofTemplateMacos());
	ofProject project;
	project.templates.emplace_back(conf.ofTemplates.back());

	// now parse project addons, or yml
	fs::path addonsListFile { conf.projectPath / "addons.make" };
	if (fs::exists(addonsListFile)) {

		vector<std::string> addonsList { textToVector(addonsListFile) };
		// vector<std::string> addonsList = { "ofxMidi" }; //ofxMidi ofxOpenCv

		for (auto & l : addonsList) {
			divider();

			ofAddon addon;
			addon.name = l;
			// check if local addon exists, if not check in of addons folder.
			if (fs::exists(conf.projectPath / l)) {
				addon.path = conf.projectPath / l;
			} else {
				if (fs::exists(conf.ofPath / "addons" / l)) {
					addon.path = conf.ofPath / "addons" / l;
				}
			}

			if (std::empty(addon.path)) {
				continue;
			}

			addon.load();
			conf.ofAddons.emplace_back(addon);
		}
	}
}

void parseConfigAllAddons() {
	alert("parseConfig begin");
	for (auto const & d : fs::directory_iterator { conf.ofPath / "addons" }) {
		if (fs::is_directory(d.path())) {
			// parseAddon(d.path());
			//
			ofAddon addon;
			addon.name = d.path().filename();
			// check if local addon exists, if not check in of addons folder.
			addon.path = d.path();
			addon.load();
			conf.ofAddons.emplace_back(addon);
		}
	}
	alert("parseConfig end");
}

void createTemplates() {
	std::vector<std::string> templateNames { "zed", "macos" };
	for (const auto & t : templateNames) {
		if (t == "zed") {
			conf.ofTemplates.emplace_back(new ofTemplateZed());
		} else if (t == "macos") {
			conf.ofTemplates.emplace_back(new ofTemplateMacos());
		}
		// etc.
	}
	alert("createTemplates", 92);
	for (auto & t : conf.ofTemplates) {
		cout << t->name << endl;
		cout << t->path << endl;
	}
	cout << conf.ofTemplates.size() << endl;
}

void ofTemplateMacos::load() {
	auto projectName = conf.projectPath.filename().string();
	fs::path xcodeProject { conf.projectPath / (projectName + ".xcodeproj") };
	cout << xcodeProject << endl;

	try {
		fs::create_directories(xcodeProject);
	} catch (const std::exception & e) {
		std::cerr << "Error creating directories: " << e.what() << std::endl;
		return false;
	}

	std::pair<string, string> rootReplacements;

	// Just replace ofPath if it is not default relative to project
	if (!fs::equivalent(conf.ofPath, "../../..")) {
		rootReplacements = { "../../..", conf.ofPath.string() };
	}

	copyTemplateFiles.push_back(
		{ path / "emptyExample.xcodeproj" / "project.pbxproj", xcodeProject / "project.pbxproj",
			{ { "emptyExample", projectName },
				rootReplacements } });

	copyTemplateFiles.push_back({ path / "Project.xcconfig",
		conf.projectPath / "Project.xcconfig",
		{ rootReplacements } });

	// try to copy all files from macos / ios - if they exist in the template
	for (auto & f : {
			 "openFrameworks-Info.plist",
			 "of.entitlements",
			 "ofxiOS-Info.plist",
			 "ofxiOS_Prefix.pch" }) {

		if (fs::exists(path / f)) {
			copyTemplateFiles.push_back({ path / f, conf.projectPath / f });
		}
	}

	copyTemplateFiles.push_back({ path / "mediaAssets", conf.projectPath / "mediaAssets" });
	copyTemplateFiles.back().isFolder = true;

	// Equivalent to SaveScheme in projectGenerator
	//
	auto schemeFolder = conf.projectPath / ( projectName + ".xcodeproj" ) / "xcshareddata/xcschemes";

	if (fs::exists(schemeFolder)) {
		fs::remove_all(schemeFolder);
	}
	fs::create_directories(schemeFolder);

	if ( target == "osx" || target == "macos" ) {
		for (auto & f : { "Release", "Debug" }) {
			copyTemplateFiles.push_back({
				path / ("emptyExample.xcodeproj/xcshareddata/xcschemes/emptyExample " + string(f) + ".xcscheme"),
				schemeFolder / (projectName + " " +f+ ".xcscheme"),
				{{ "emptyExample", projectName }}
			});
		}

		copyTemplateFiles.push_back({
			conf.projectPath / (projectName + ".xcodeproj/project.xcworkspace"),
			path / "emptyExample.xcodeproj/project.xcworkspace"
		});
	} else {

		// MARK:- IOS sector;
		copyTemplateFiles.push_back({
			path / "emptyExample.xcodeproj/xcshareddata/xcschemes/emptyExample.xcscheme",
			schemeFolder / (projectName + ".xcscheme"),
			{{ "emptyExample", projectName }}
		});
	}
}
