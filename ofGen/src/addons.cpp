

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
	for (auto & f : filesMap) {
		for (auto & s : f.second) {
			s = fs::relative(s, path);
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
					// std::cout << s << std::endl;
					// alert("added " + s.string(), 92);
					alert("	└─excluded " + s.string(), 0);
					alert("	   exclusion=" + e.string() + ", section=" + f.first, 90);
					// alert (, 93);
					continue;
				}
				// alert("	└─not excluded " + s.string(), 93);
				// alert("	   exclusion=" + e.string() + ", section=" + f.first, 33);
			}
			if (add) {
				// cout << "adding " << s << endl;
				filteredMap[f.first].emplace_back(s);
			}
		}

		// Sort alphabetically everything.
		std::sort(filteredMap[f.first].begin(), filteredMap[f.first].end(), [](const fs::path & a, const fs::path & b) {
			return a.string() < b.string();
		});
	}

	// for (auto & e : exclusionsMap) {
	// 	alert(">> addon exclusion filter : " + e.first, 92);
	// 	cout << e.second.size() << endl;
	// 	for (auto & s : e.second) {
	// 		alert(s, 90);
	// 	}
	// }

	// for (auto & e : filteredMap) {
	// 	alert(">> filtered list : " + e.first, 92);
	// 	cout << e.second.size() << endl;
	// 	for (auto & s : e.second) {
	// 		alert(s, 90);
	// 	}
	// }

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
	alert("	showFiles", 34);
	for (auto & f : filesMap) {
		alert(f.first + ":", 31);
		for (auto & s : f.second) {
			std::cout << s << std::endl;
		}
	}
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
			// stringReplace(line, ":", "");
			currentParseState = line;
		}

		// alert (">> currentParseState " + currentParseState, 93);
		bool consider = currentParseState == "common:" || currentParseState == "macos:" || currentParseState == "osx:";

		// if (  currentParseState != "common"
		//           && currentParseState != "macos"
		//           && currentParseState != "osx"
		// //           // && currentParseState != "emscripten"
		// ) {
		// break;
		// }

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
				string value = stringReplace(a, "%", "");
				// alert(value, 92);
				exclusionsMap[e.second].emplace_back(value);
			}
		} else {
			// alert(e.first + " empty");
		}
	}
	//std::map<string, vector<string> > addonProperties;
}

void gatherProjectInfo() {
	alert("gatherProjectInfo", 92);
	// Add project files. TODO: additional source folders
	ofProject project;


	// cout << conf.templateNames.size() << endl;
	// for (auto & t : conf.templateNames) {
	// 	alert(t, 95);
	// }

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

	fs::path addonsListFile { conf.projectPath / "addons.make" };
	if (fs::exists(addonsListFile)) {
		vector<std::string> addonsList { textToVector(addonsListFile) };
		// vector<std::string> addonsList = { "ofxMidi" }; //ofxMidi ofxOpenCv

		for (auto & l : addonsList) {

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

	// pass files to projects.
	project.build();
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

// void buildTemplates() {
// 	alert("buildTemplates()", 95);
// 	for (auto & t : conf.templates) {
// 		// 		t->info();
// 		t->build();
// 	}
// }

// void createTemplates() {
// 	// std::vector<std::string> templateNames { "zed", "macos" };
// 	std::vector<std::string> templateNames { "zed", "make" };
// 	for (const auto & t : templateNames) {
// 		if (t == "zed") {
// 			conf.templates.emplace_back(new ofTemplateZed());
// 		} else if (t == "macos") {
// 			conf.templates.emplace_back(new ofTemplateMacos());
// 		}
// 		// etc.
// 	}
// 	alert("createTemplates", 92);
// 	for (auto & t : conf.templates) {
// 		cout << t->name << " : " << t->path << endl;
// 		t->load();
// 		// t->info();
// 	}
// 	// cout << conf.ofTemplates.size() << endl;
// }

void ofProject::build() {
	divider();
	alert("ofProject::build", 92);
	// std::cout << "addons.size " << addons.size() << std::endl;
	// std::cout << "templates.size " << templates.size() << std::endl;

	// each template for specific project
	for (auto & t : templates) {
		// each addon for specific project
		for (auto & a : addons) {
			alert("	ofProject::addAddon " + a->name, 34);
			t->addAddon(a);
		}
		t->build();
		t->save();
		// Pass addons list of filtered files to template
		// Pass addons key:value of properties to template
		// t->
	}
}
