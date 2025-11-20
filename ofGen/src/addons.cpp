#include "addons.h"
#include "ofTemplateVSCode.h"
#include "templates.h"
// #include "ofTemplateVisualStudio.h"
// #include "ofTemplateMake.h"

void ofAddon::scanFolder(const fs::path & path,
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
		// cout << f << endl;

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
				// FIXME: This is maybe unneded... test without it with multiple addons
				if (!isProject) {
					filesMap["includes"].emplace_back(f);
				}
			}
		} else {
			if (ext == ".a" || ext == ".lib") {
				filesMap["libs"].emplace_back(f);
			} else if (ext == ".dylib" || ext == ".so" || ext == ".dll") {
				filesMap["sharedLibs"].emplace_back(f);
			} else if (ext == ".h" || ext == ".hpp" || ext == ".tcc" || ext == ".inl" || ext == ".in") {

				filesMap["headers"].emplace_back(f);
			} else if (ext == ".c" || ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".m" || ext == ".mm") {
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
					alert("	└─ excluded " + s.string(), 0);
					alert("	    exclusion=" + e.string() + ", section=" + f.first, 90);
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
	alert("	loadFiles " + path.string(), 34);

	for (auto & p : addonProperties["ADDON_LIBS"]) {
		alert("-> addon libs " + p, 35);
		filesMap["libs"].emplace_back(p);
	}

	for (auto & p : addonProperties["ADDON_INCLUDES"]) {
		alert("-> addon includes " + p, 35);
		filesMap["includes"].emplace_back(p);
	}

	// test 12 oct.
	for (auto & p : addonProperties["ADDON_DEFINES"]) {
		alert("-> addon defines " + p, 35);
		conf.defines.emplace_back(p);
	}

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
				for (const auto & p : conf.platforms) {
					fs::path folder { f / "lib" / p };
					if (!fs::exists(folder)) {
						alert("		folder don't exist " + folder.string(), 96);
						continue;
					} else {
						hasPlatformFolder = true;
						scanFolder(folder, filesMap, true);
						alert("		folder yes exists " + folder.string(), 94);
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

			// if is project, add libs and include folders for each lib.
			else {
				if (isProject) {
					alert("		isProject=true, adding lib and include folders for each lib. " + folderLibs.string(), 35);

					if (fs::exists(f / "lib")) {
						scanFolder(f / "lib", filesMap, true);
					}
					if (fs::exists(f / "include")) {
						scanFolder(f / "include", filesMap, true);
					}
				}
			}
		}

		if (!hasPlatformFolder) {
			// if isProject is true, libs are added already
			if (!isProject) {
				alert("		platform folder not found, will scan everything " + folderLibs.string(), 35);
				scanFolder(folderLibs, filesMap, true);
			}
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
	// int lineNum = 0;

	for (auto & originalLine : textToVector(addonConfig)) {
		// lineNum++;
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
		// line = stringReplace(line, "\\$(OF_ROOT)", conf.ofPath.string());

		replaceAll(line, "$(OF_ROOT)", conf.ofPath.string());
		// cout << line << endl;
		// Trim., removing whitespace
		// line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());

		if (line[line.size() - 1] == ':') {
			// stringReplace(line, ":", "");
			currentParseState = line;
		}

		// alert (">> currentParseState " + currentParseState, 93);
		// FIXME: consider other Platforms soon.
		bool consider = currentParseState == "common:" ||
			// currentParseState == "macos:" ||
			// currentParseState == "osx:";
			currentParseState == conf.platforms[0] + ":";
		if (consider) {
			// FIXME: Remove
			// alert("currentParseState " + currentParseState, 95);
			if (line.find("=") != string::npos) {
				vector<string> varValue;
				bool limpa = false;
				if (line.find("+=") != string::npos) {
					varValue = ofSplitString(line, "+=");
				} else {
					limpa = true;
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
					// FIXME: Remove
					// alert("---- variable " + variable, 92);
					// alert("---- value " + value, 92);
				}
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
			// addon->name = d.path().filename();
			addon->name = d.path().filename().string();
			// check if local addon exists, if not check in of addons folder.
			addon->path = d.path();
			addon->load();
			conf.addons.emplace_back(addon);
		}
	}
	alert("parseConfig end");
}

bool buildProject() {
	// ofProject project;

	bool hasYml = conf.loadYML();
	if (hasYml) {
		alert("of.yml found, building from there", 95);
	} else {
		alert("building from addons.make", 95);
		fs::path addonsListFile { conf.projectPath / "addons.make" };
		if (fs::exists(addonsListFile)) {
			conf.addonsNames = textToVector(addonsListFile);
		} else {
			alert("no addons.make found", 95);
		}

		alert("No templates found, ofgen will deduce from platform", 95);

		std::map<std::string, std::vector<std::string>> platformTemplates {
			{ "macos", { "macos", "chalet", "zed" } },
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
			if (empty(conf.templateNames)) {
				conf.templateNames = platformTemplates[platform];
			} else {
			}
		}

		alert("Templates ");
		cout << joinStrings(conf.templateNames, ", ") << endl;
	}

	if (!conf.isValidOfPath()) {
		alert("OF not found in default path " + conf.ofPath.string());
		conf.help();
		return false;
	} else {
		alert("of path OK, proceeding");

		if (!fs::exists("bin")) {
			alert("bin folder not found, creating");
			fs::create_directory("bin");
		}
	}

	// scanFolder()
	// create templates, add to project
	for (auto & t : conf.templateNames) {
		if (t == "chalet") {
			conf.templates.emplace_back(new ofTemplateChalet());
			conf.project.templates.emplace_back(conf.templates.back());
		} else if (t == "zed") {
			conf.templates.emplace_back(new ofTemplateZed());
			conf.project.templates.emplace_back(conf.templates.back());
		} else if (t == "macos") {
			conf.templates.emplace_back(new ofTemplateMacos());
			conf.project.templates.emplace_back(conf.templates.back());
		} else if (t == "vscode") {
			conf.templates.emplace_back(new ofTemplateVSCode());
			conf.project.templates.emplace_back(conf.templates.back());
		}
		// else if (t == "make") {
		// 	conf.templates.emplace_back(new ofTemplateMake());
		// 	project.templates.emplace_back(conf.templates.back());
		// }

		// else if (t == "visualstudio") {
		// 	conf.templates.emplace_back(new ofTemplateVisualStudio());
		// 	project.templates.emplace_back(conf.templates.back());
		// }

		else {
			alert("invalid template name : " + t + ", exiting", 95);
			return false;
			// std::exit(1);
		}

		if (empty(conf.openCommand) && !empty(conf.templates.back()->openCommand)) {
			conf.openCommand = conf.templates.back()->openCommand;
		}

		if (empty(conf.buildCommand) && !empty(conf.templates.back()->buildCommand)) {
			conf.buildCommand = conf.templates.back()->buildCommand;
		}

		if (empty(conf.runCommand) && !empty(conf.templates.back()->runCommand)) {
			conf.runCommand = conf.templates.back()->runCommand;
		}
	}

	// load templates, show info of each template
	// for (auto & t : conf.templates) {
	// 	// cout << t->name << " : " << t->path << endl;
	// 	t->load();
	// 	t->info();
	// }

	// now parse project addons, or yml

	if (!fs::exists("./src")) {
		// FIXME: check if template is ios and copy mm files accordingly. if not copy src files from templates.
		fs::path from { conf.ofPath / "scripts" / "templates" / "src" };
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
			conf.addons.push_back(new ofAddon());
			ofAddon * addon = conf.addons.back();
			addon->isProject = true;
			addon->name = "ProjectSourceFiles_" + conf.projectName;
			addon->path = "";

			for (auto & f : conf.frameworks) {
				addon->filesMap["frameworks"].emplace_back(f);
			}

			// addon->showFiles();
			// addon->info();
			for (auto & path : conf.additionalSources) {
				addon->filesMap["includes"].emplace_back(path);
			}
			addon->load();
			// conf.addons.emplace_back(addon);
			conf.project.addons.emplace_back(conf.addons.back());
		}

		// TODO: Add here additional sources
		// for (auto & a : conf.additionalSources) {
		// 	alert(">> Additional Sources Folder: " + a.string(), 95);
		// 	conf.addons.push_back(new ofAddon());
		// 	ofAddon * addon = conf.addons.back();
		// 	addon->isProject = true;
		// 	addon->name = "AdditionalSource_" + conf.projectName;
		// 	addon->path = a;
		// 	addon->isProject = true;

		// 	scanFolder(a, addon->filesMap, true);
		// 	addon->load();
		// 	project.addons.emplace_back(conf.addons.back());
		// }
	}
	// else {
	// 	alert("NO SRC FILE FOUND IN PROJECT", 95);
	// 	std::exit(1);
	// }

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
			conf.project.addons.emplace_back(conf.addons.back());
		}
	}
	// }

	// pass files to projects.
	conf.project.build();

	return true;
}
