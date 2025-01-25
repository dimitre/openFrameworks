#include "addons.h"
#include <iostream>
#include <fstream>

using std::string;
using std::vector;
#include "utils.h"

void ofAddon::load() {
	alert("ADDON " + name, 91);

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
			alert(f.string(), 95);
			auto includeFolder { f / "include" };
			if (fs::exists(includeFolder)) {
				conf.scanFolder(includeFolder, true);
			}

			if (fs::exists(f / "lib")) {
				for (const std::string p : conf.platforms) {

					fs::path folder { f / "lib" / p };
					if (!fs::exists(folder)) {
						alert("folder don't exist " + folder.string(), 96);
						continue;
					} else {
						hasPlatformFolder = true;
						alert("folder yes exist " + folder.string(), 94);
					}
				}
			}
		}

		if (!hasPlatformFolder) {
			alert("don't have platform folder, will scan everything " + folderLibs.string(), 92);
			conf.scanFolder(folderLibs, true);
		}
	}

	// if (fs::exists(libsPath)) {
	// 	for (auto const & d : fs::directory_iterator { libsPath }) {
	// 		if (fs::is_directory(d.path())) {

	// 			alert("		Lib inside: " + d.path().string(), 33);

	// 			// TODO: specify platform or platforms before.
	// 			fs::path staticLibsFolder { d.path() / "lib" / "macos" };
	// 			if (fs::exists(staticLibsFolder)) {
	// 				for (auto const & d : fs::directory_iterator { staticLibsFolder }) {
	// 					alert("		" + d.path().string(), 34);
	// 				}
	// 			}

	// 			staticLibsFolder = { d.path() / "lib" / "osx" };
	// 			if (fs::exists(staticLibsFolder)) {
	// 				for (auto const & d : fs::directory_iterator { staticLibsFolder }) {
	// 					alert("		" + d.path().string(), 34);
	// 				}
	// 			}

	// 			fs::path includesFolder { d.path() / "include" };
	// 			if (fs::exists(includesFolder)) {

	// 				fs::recursive_directory_iterator it { includesFolder };
	// 				fs::recursive_directory_iterator last {};

	// 				for (; it != last; ++it) {
	// 					if (fs::is_directory(it->path())) {
	// 						alert("		" + it->path().string(), 35);
	// 						// folderListMap[path].emplace_back(it->path());
	// 					}
	// 				}
	// 			}
	// 		}
	// 	}
	// } else {
	// 	// alert(" libsPath not found ", 31);
	// }

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
		// not sure if it will work. I'm replacing with spaces. I need to remove them
		// std::replace( line.begin(), line.end(), '\r', ' ');
		// std::replace( line.begin(), line.end(), '\n', ' ');
		// stringReplace(line,"\n","");

		line = ofTrim(line);

		// discard comments and blank lines
		if (line[0] == '#' || line == "") {
			continue;
		}
		// alert (line, 31);
		stringReplace(line, " \\= ", "=");
		stringReplace(line, "\\= ", "=");
		stringReplace(line, " \\=", "=");
		stringReplace(line, " \\+\\= ", "+=");
		stringReplace(line, " \\+\\=", "+=");
		stringReplace(line, "\\+\\= ", "+=");
		// stringReplace(line,"ADDON_LIBS","SEXOANAL");
		// alert (line, 32);

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

	for (auto & a : addonProperties) {
		alert("    	" + a.first, 94);
		for (auto & p : a.second) {
			alert("     	  " + p, 95);
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
			// straight copy
			try {
				fs::copy(from, to, fs::copy_options::update_existing);
			} catch (fs::filesystem_error & e) {
				std::cerr << "error copying template file " << from << " : " << to << std::endl;
				std::cerr << e.what() << std::endl;
				return false;
			}
		}
	} else {
		return false;
	}
	return true;
}





void genConfig::scanFolder(const fs::path & path, bool recursive) {
	alert("scanFolder " + path.string(), 97);
	// it should exist and be a folder.
	if (!fs::exists(path)) return;
	if (!fs::is_directory(path)) return;

	filesMap["includes"].emplace_back(path);

	// fs::recursive_directory_iterator it { path };
	// fs::recursive_directory_iterator last {};
	// for (; it != last; ++it) {

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

		alert(f);

		auto ext = f.extension().string();
		if (fs::is_directory(f)) {
			// if (ext == ".xcodeproj") {
			// 	it.disable_recursion_pending();
			// 	continue;
			// }

			if (ext == ".framework" || ext == ".xcframework") {
				// ADD To Frameworks List, and stop searching inside this directory
				filesMap["frameworks"].emplace_back(f);
				it.disable_recursion_pending();
				continue;
			} else {
				// ADD To includes list
				filesMap["includes"].emplace_back(f);
			}

		} else {
			if (ext == ".a" || ext == ".lib") {
				filesMap["libs"].emplace_back(f);
			}
			// allFiles.libs.emplace_back(f);
			else if (ext == ".dylib" || ext == ".so" || ext == ".dll") {
				filesMap["sharedLibs"].emplace_back(f);
			} else if (ext == ".h" || ext == ".hpp" || ext == ".m" || ext == ".tcc" || ext == ".inl") { //is tcc correct here?
				filesMap["headers"].emplace_back(f);
			} else if (ext == ".c" || ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".mm") {
				filesMap["sources"].emplace_back(f);
			} else {
				alert("no desired extension " + f.string());
			}
		}
	}
}




void gatherProjectInfo() {
	alert("gatherProjectInfo", 92);
	// Add project files. TODO: additional source folders
	// std::cout << &conf << std::endl;

	conf.ofTemplates.emplace_back(new ofTemplateMacos());
	ofProject project;
	project.templates.emplace_back(conf.ofTemplates.back());

	// now parse project addons, or yml
	fs::path addonsListFile { conf.projectPath / "addons.make" };
	if (fs::exists(addonsListFile)) {
	// for (auto & l : { "ofxMidi" }) {

	   for (auto & l : textToVector(addonsListFile)) {
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

			// conf.scanFolder(addon.path / "src", true);

			// scan, but not recursive, so add paths to paths, sources to sources.
			// conf.scanFolder(addon.path / "libs", false);


			// std::cout << dir_entry.path() << '\n';

			addon.load();
			// cout << l << endl;
			// parseAddon(l);

			conf.ofAddons.emplace_back(addon);
		}
	}

	// conf.scanFolder(conf.projectPath / "src", true);

}



void parseConfigAllAddons() {
	alert("parseConfig begin");
	for (auto const & d : fs::directory_iterator { conf.ofPath / "addons" }) {
		if (fs::is_directory(d.path())) {
			// parseAddon(d.path());
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
