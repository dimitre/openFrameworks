

#include "addons.h"
#include "templates.h"

#include <fstream>
#include <iostream>

// using std::cout;
// using std::endl;
// using std::string;
// using std::vector;

void scanFolder(const fs::path & path,
	std::map<std::string, std::vector<fs::path>> & filesMap,
	bool recursive) {
	// it should exist and be a folder.
	if (!fs::exists(path)) return;
	if (!fs::is_directory(path)) return;
	alert("	scanFolder " + path.string(), 92);

	// do we want to add all root paths to includes or not?
	filesMap["includes"].emplace_back(path);
	alert("	add includes: " + path.string(), 34);

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
		auto ext = f.extension().string();

		if (fs::is_directory(f)) {
			if (ext == ".framework" || ext == ".xcframework") {
				// ADD To Frameworks List, and stop searching inside this directory

				alert("		Adding Framework " + f.string(), 95);
				filesMap["frameworks"].emplace_back(f);
				it.disable_recursion_pending();
				continue;
			} else {
				// ADD To includes list, keep iterating
				filesMap["includes"].emplace_back(f);
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

void ofAddon::load() {
	divider();
	alert("ofAddon :: " + name, 92);
	loadAddonConfig();
	loadFiles();
	relative();
	refine();
	// showFiles();
}

void ofAddon::relative() {
	alert("	relative", 34);
	if (!isProject) {
		for (auto & f : filesMap) {
			for (auto & s : f.second) {
				s = fs::relative(s, path);
			}
		}
	}
}

void ofAddon::refine() {
	alert("	refine", 34);
	// With this we copy rules from ADDON_SOURCES_EXCLUDE to header files.
	exclusionsMap["headers"] = exclusionsMap["sources"];

	for (const auto & f : filesMap) {
		for (const auto & s : f.second) {
			bool add = true;
			for (const auto & e : exclusionsMap[f.first]) {
				if (ofIsPathInPath(s, e)) {
					add = false;
					alert("	└─excluded " + s.string(), 0);
					alert("	   exclusion=" + e.string() + ", section=" + f.first, 90);
					continue;
				}
			}
			if (add) {
				filteredMap[f.first].emplace_back(s);
			}
		}

		// Sort alphabetically everything.
		std::sort(filteredMap[f.first].begin(), filteredMap[f.first].end(), [](const fs::path & a, const fs::path & b) {
			return a.string() < b.string();
		});
	}
}

void ofAddon::showFiles() {
	alert("	showFiles", 34);

	for (auto & s : exclusionsMap["includes"]) {
		alert("		exclusion: " + s.string(), 95);
	}

	for (auto & s : filteredMap["includes"]) {
		alert("		include: " + s.string(), 96);
	}
	// for (auto & f : filteredMap) {
	// 	alert(f.first + ":", 32);
	// 	for (auto & s : f.second) {
	// 		std::cout << "       " << s << std::endl;
	// 	}
	// }
}

void ofAddon::loadFiles() {
	alert("	loadFiles", 34);

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
			alert("		" + f.string(), 35);

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

			// Avoid scanning duplicate if we are going to scan all libsFolder recursively later
			if (hasPlatformFolder) {
				{
					auto folder { f / "include" };
					if (fs::exists(folder)) {
						scanFolder(folder, filesMap, true);
					}
				}
				// special thing for ofxKinect
				{
					auto folder { f / "src" };
					if (fs::exists(folder)) {
						scanFolder(folder, filesMap, true);
					}
				}
			}
		}

		if (!hasPlatformFolder) {
			alert("		don't have platform folder, will scan everything " + folderLibs.string(), 35);
			scanFolder(folderLibs, filesMap, true);
		}
	}

	// for (auto & f : filesMap) {
	// 	cout << f.first << endl;
	// 	for (auto & a : f.second) {
	// 		cout << a << endl;
	// 	}
	// }
}

void ofAddon::loadAddonConfig() {
	// alert("	loadAddonConfig :: addon " + name, 92);

	fs::path addonConfig { path / "addon_config.mk" };
	// alert ("zed " + fileName.string(), 91);
	if (!fs::exists(addonConfig)) {
		// alert(" addonConfig not found " + addonConfig.string(), 31);
		return;
	} else {
		alert("	loadAddonConfig found " + addonConfig.string(), 90);
	}

	// FIXME: transformar para textToString, fazer os replaces todos de uma vez só.
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
		line = stringReplace(line, "\\$(OF_ROOT)", conf.ofPath.string());

		// Trim., removing whitespace
		// line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());

		if (line[line.size() - 1] == ':') {
			// stringReplace(line, ":", "");
			currentParseState = line;
		}

		// alert (">> currentParseState " + currentParseState, 93);
		// FIXME: consider other Platforms soon.
		bool consider = currentParseState == "common:" || currentParseState == "macos:" || currentParseState == "osx:";

		if (consider)
			if (line.find("=") != string::npos) {
				bool addToValue = false;
				vector<string> varValue;
				bool limpa = false;
				if (line.find("+=") != string::npos) {
					addToValue = true;
					// FIXME: maybe not needed. a simple split is ok.
					// varValue = splitStringOnceByLeft(line, "+=");
					varValue = ofSplitString(line, "+=");
				} else {
					limpa = true;
					addToValue = false;
					// varValue = splitStringOnceByLeft(line, "=");
					varValue = ofSplitString(line, "=");
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

	const static std::map<std::string, std::string> exclusionsType {
		{ "ADDON_SOURCES_EXCLUDE", "sources" },
		{ "ADDON_INCLUDES_EXCLUDE", "includes" },
		{ "ADDON_LIBS_EXCLUDE", "libs" },
	};

	for (auto & e : exclusionsType) {
		if (addonProperties.contains(e.first)) {
			// alert(e.first + " not empty");
			for (auto & a : addonProperties[e.first]) {
				string value = stringReplace(a, "%", "");
				//alert("exclusions " + e.first + " : " + value, 95);
				exclusionsMap[e.second].emplace_back(value);
			}
		} else {
			// alert(e.first + " empty");
		}
	}
}

void parseConfigAllAddons() {
	alert("parseConfig begin");
	for (auto const & d : fs::directory_iterator { conf.ofPath / "addons" }) {
		if (fs::is_directory(d.path())) {
			// parseAddon(d.path());
			//
			conf.addons.push_back(new ofAddon());
			ofAddon * addon = conf.addons.back();

			// ofAddon addon;
			addon->name = d.path().filename();
			// check if local addon exists, if not check in of addons folder.
			addon->path = d.path();
			addon->load();
			conf.addons.emplace_back(addon);
		}
	}
	alert("parseConfig end");
}

void ofProject::build() {
	divider();
	alert("ofProject::build", 92);
	// std::cout << "addons.size " << addons.size() << std::endl;
	// std::cout << "templates.size " << templates.size() << std::endl;

	// each template for specific project
	for (auto & t : templates) {
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

void gatherProjectInfo() {
	alert("gatherProjectInfo", 92);
	// Add project files. TODO: additional source folders
	ofProject project;

	bool hasYml = conf.loadYML();
	if (hasYml) {
		alert("of.yml found, building from there", 95);
	} else {
		alert("of.yml not found, building from addons.make", 95);

		fs::path addonsListFile { conf.projectPath / "addons.make" };
		if (fs::exists(addonsListFile)) {
			conf.addonsNames = textToVector(addonsListFile);
		}
	}

	if (!conf.isValidOfPath()) {
		alert("OF not found in default path " + conf.ofPath.string());
		conf.help();
		return;
	} else {
		alert("of path OK, proceeding");
	}
	// scanFolder()
	// create templates, add to project
	for (auto & t : conf.templateNames) {
		if (t == "macos") {
			conf.templates.emplace_back(new ofTemplateMacos());
			project.templates.emplace_back(conf.templates.back());
		} else if (t == "zed") {
			conf.templates.emplace_back(new ofTemplateZed());
			project.templates.emplace_back(conf.templates.back());
		} else if (t == "make") {
			conf.templates.emplace_back(new ofTemplateMake());
			project.templates.emplace_back(conf.templates.back());
		} else if (t == "vscode") {
			conf.templates.emplace_back(new ofTemplateVSCode());
			project.templates.emplace_back(conf.templates.back());
		}
	}

	// load templates, show info of each template
	for (auto & t : conf.templates) {
		// cout << t->name << " : " << t->path << endl;
		t->load();
		t->info();
	}

	// now parse project addons, or yml

	// DELICATE. treating projects as an addon.
	if (fs::exists("./src")) {
		conf.addons.push_back(new ofAddon());
		ofAddon * addon = conf.addons.back();
		addon->isProject = true;
		addon->name = "ProjectSourceFiles_" + conf.projectName;

		// addon->showFiles();
		addon->path = "";
		addon->load();

		// addon->info();
		// conf.addons.emplace_back(addon);
		project.addons.emplace_back(conf.addons.back());
	} else {
		alert("NO SRC FILE FOUND IN PROJECT", 95);
	}



	// fs::path addonsListFile { conf.projectPath / "addons.make" };
	// if (fs::exists(addonsListFile)) {
	// vector<std::string> addonsList { textToVector(addonsListFile) };
	// vector<std::string> addonsList = { "ofxMidi" }; //ofxMidi ofxOpenCv

	for (auto & l : conf.addonsNames) {
		if (l != "") {
			conf.addons.push_back(new ofAddon());
			ofAddon * addon = conf.addons.back();

			// ofAddon addon;
			addon->name = l;
			// check if local addon exists, if not check in of addons folder.
			if (fs::exists(conf.projectPath / l)) {
				addon->path = conf.projectPath / l;
			} else {
				if (fs::exists(conf.ofPath / "addons" / l)) {
					addon->path = conf.ofPath / "addons" / l;
				}
			}

			if (std::empty(addon->path)) {
				continue;
			}

			addon->load();
			// conf.addons.emplace_back(addon);
			project.addons.emplace_back(conf.addons.back());
		}
	}
	// }

	// pass files to projects.
	project.build();
}
