#include "templates.h"
#include "addons.h"
#include "uuidxx.h"
#include <nlohmann/json.hpp>
// #include <iostream> //in utils
#include <fstream> //in utils

using nlohmann::json;

string ofTemplateMacos::addFile(const fs::path & path, const fs::path & folder, const fileProperties & fp) {
	string UUID { "" };
	//	alert("xc::addFile " + path.string() + " :folder:" + folder.string(), 31);
	{
		string fileType { "file" };
		fileType = extensionToFileType[path.extension()];

		if (fileType == "") {
			if (fs::is_directory(path) || fp.isGroupWithoutFolder) {
				fileType = "folder";
			} else {
				// Break here if fileType is not set. and it is not a folder
				return {};
			}
		}
		UUID = generateUUID(path);

		// This is adding a file. any file.
		addCommand("Add :objects:" + UUID + ":fileEncoding string 4");
		if (fp.isGroupWithoutFolder) {
			addCommand("Add :objects:" + UUID + ":isa string PBXGroup");
		} else {
			addCommand("Add :objects:" + UUID + ":isa string PBXFileReference");
		}
		addCommand("Add :objects:" + UUID + ":lastKnownFileType string " + fileType);
		addCommand("Add :objects:" + UUID + ":name string " + ofPathToString(path.filename()));

		if (fp.absolute || fp.isRelativeToSDK || path.is_absolute()) { //
			// FIXME: Still some confusion here about relative to source or absolute.

			if (fp.isRelativeToSDK) {
				// std::string frameworkPath { "System/Library/Frameworks/" + ofPathToString(path) + ".framework" };
				// addCommand("Add :objects:" + UUID + ":path string " + frameworkPath);
				addCommand("Add :objects:" + UUID + ":sourceTree string SDKROOT");

			} else {
				addCommand("Add :objects:" + UUID + ":sourceTree string SOURCE_ROOT");
			}
			addCommand("Add :objects:" + UUID + ":path string " + ofPathToString(path));
			// addCommand("Add :objects:" + UUID + ":sourceTree string <absolute>");

			// FIXME: REVIEW
			// if (fs::exists(conf.projectPath / path)) {
			// 	addCommand("Add :objects:" + UUID + ":path string " + ofPathToString(path));
			// }
		} else {
			addCommand("Add :objects:" + UUID + ":path string " + ofPathToString(path.filename()));
			addCommand("Add :objects:" + UUID + ":sourceTree string <group>");
		}

		// Eventually remove isFolder and base parameter
		std::string folderUUID { getFolderUUID(folder, path) };

		// addCommand("# ---- addFileToFolder UUID : " + ofPathToString(folder));
		addCommand("Add :objects:" + folderUUID + ":children: string " + UUID);

		string buildUUID { generateUUID(ofPathToString(path) + "-build") };
		// If any other option is true, add buildUUID entries.
		if (
			fp.addToBuildPhase || fp.codeSignOnCopy || fp.copyFilesBuildPhase || fp.addToBuildResource || fp.addToResources
			//|| fp.frameworksBuildPhase ~ I've just removed this one, favoring -InFrameworks
		) {
			addCommand("# ---- addToBuildPhase " + buildUUID);
			addCommand("Add :objects:" + buildUUID + ":isa string PBXBuildFile");
			addCommand("Add :objects:" + buildUUID + ":fileRef string " + UUID);
		}

		if (fp.addToBuildPhase) { // Compile Sources
			// Not sure if it applies to everything, applies to srcFile.
			addCommand("# ---- addToBuildPhase");
			// addCommand("Add :objects:" + buildActionMaskUUID + ":files: string " + buildUUID);
			addCommand("Add :objects:" + uuid["buildActionMask"] + ":files: string " + buildUUID);
		}

		if (fp.copyFilesBuildPhase) {
			// If we are going to add xcframeworks to copy files -> destination frameworks, we should include here
			if (fileType == "wrapper.framework" || fileType == ".xcframework") {
				// copy to frameworks
				addCommand("# ---- copyPhase Frameworks " + buildUUID);
				addCommand("Add :objects:E4C2427710CC5ABF004149E2:files: string " + buildUUID);
			} else {
				// copy to executables
				addCommand("# ---- copyPhase Executables " + buildUUID);
				addCommand("Add :objects:E4A5B60F29BAAAE400C2D356:files: string " + buildUUID);
			}
		}

		if (fp.codeSignOnCopy) {
			// addCommand("# ---- codeSignOnCopy " + buildUUID);
			addCommand("Add :objects:" + buildUUID + ":settings:ATTRIBUTES array");
			addCommand("Add :objects:" + buildUUID + ":settings:ATTRIBUTES: string CodeSignOnCopy");
		}

		if (fp.addToBuildResource) {
			addCommand("# ---- addToBuildResource / mediaAssets");
			addCommand("Add :objects:" + uuid["mediaAssets"] + ":files: string " + UUID);
		}

		if (fp.addToResources) {
			// FIXME: test if it is working on iOS
			if (uuid["resources"] != "") {
				addCommand("# ---- addToResources (IOS only) ?" + buildUUID);
				addCommand("Add :objects:" + uuid["resources"] + ": string " + buildUUID);
			}
		}

		if (fp.frameworksBuildPhase) { // Link Binary With Libraries
			auto tempUUID = generateUUID(ofPathToString(path) + "-InFrameworks");
			addCommand("Add :objects:" + tempUUID + ":fileRef string " + UUID);
			addCommand("Add :objects:" + tempUUID + ":isa string PBXBuildFile");

			addCommand("# --- PBXFrameworksBuildPhase");
			addCommand("Add :objects:E4B69B590A3A1756003C02F2:files: string " + tempUUID);
		}

		if (path.extension() == ".framework") {
			addCommand("# ---- Frameworks Folder " + UUID);
			addCommand("Add :objects:901808C02053638E004A7774:children: string " + UUID);

			addCommand("# ---- PBXFrameworksBuildPhase " + buildUUID);
			addCommand("Add :objects:1D60588F0D05DD3D006BFB54:files: string " + buildUUID);
		}
	}
	return UUID;
}

void ofTemplateMacos::addSrc(const fs::path & srcFile, const fs::path & folder) {
	// alert ("xcodeProject::addSrc " + ofPathToString(srcFile) + " : " + ofPathToString(folder), 31);

	string ext = ofPathToString(srcFile.extension());

	//		.reference = true,
	//		.addToBuildPhase = true,
	//		.codeSignOnCopy = false,
	//		.copyFilesBuildPhase = false,
	//		.linkBinaryWithLibraries = false,
	//		.addToBuildResource = false,
	//		.addToResources = false,

	fileProperties fp;
	fp.addToBuildPhase = true;
	fp.isSrc = true;

	if (ext == ".h" || ext == ".hpp") {
		fp.addToBuildPhase = false;
	} else if (ext == ".xib") {
		fp.addToBuildPhase = false;
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".metal") {
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".entitlements") {
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".info") {
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (target == "ios") {
		fp.addToBuildPhase = true;
		fp.addToResources = true;
	}

	string UUID {
		addFile(srcFile, folder, fp)
	};

	// if (ext == ".mm" || ext == ".m") {
	// 	addCompileFlagsForMMFile(srcFile);
	// }
}

void ofTemplateMacos::addAddon(ofAddon * a) {
	// Here we add all lists of files to project
	// and we add all flags to project.

	// ADDON_CFLAGS ADDON_CPPFLAGS ADDON_LDFLAGS ADDON_PKG_CONFIG_LIBRARIES ADDON_FRAMEWORKS ADDON_DEFINES
	// ADDON_CFLAGS e ADDON_LDFLAGS estão no ofxSyphon addon.
	// ofxHapPlayer tem varias coisas legais tambem
	// 	ADDON_LDFLAGS = -rpath @loader_path/../../../../../../../addons/ofxHapPlayer/libs
	// ADDON_INCLUDES_EXCLUDE = libs/ffmpeg/include/libavformat
	// ADDON_INCLUDES_EXCLUDE += libs/ffmpeg/include/libavutil
	// ADDON_INCLUDES_EXCLUDE += libs/ffmpeg/include/libavcodec
	// ADDON_INCLUDES_EXCLUDE += libs/ffmpeg/include/libswresample

	// ADDON_INCLUDES ADDON_LIBS ADDON_SOURCES
	//
	// for (auto & p : a->addonProperties) {
	// 	cout << p.first << endl;
	// 	for (auto & e : p.second) {
	// 		cout << " " << e << endl;
	// 	}
	// 	cout << "-----" << endl;
	// }

	for (auto & f : a->filteredMap["sources"]) {
		fs::path p = (a->path / f).parent_path();
		fs::path p2 = relative(p, conf.ofPath);
		if (a->isProject) {
			p2 = f.parent_path();
			// alert ("ADDON IS PROJECT", 95);
			// alert(f.string(), 95);
			// alert(f.filename().string(), 95);
			// alert(p2.string(), 96);
		}

		addSrc(f.filename(), p2);
	}

	for (auto & f : a->filteredMap["headers"]) {
		fs::path p = (a->path / f).parent_path();
		fs::path p2 = relative(p, conf.ofPath);

		if (a->isProject) {
			p2 = relative(p, conf.projectPath);
			p2 = f.parent_path();
		}

		addSrc(f.filename(), p2);
	}

	// ADD Include
	for (auto & f : a->filteredMap["includes"]) {
		for (auto & c : buildConfigs) {
			fs::path p = a->path / f;
			// alert ("->" + p.string(), 95);
			addCommand("Add :objects:" + c + ":buildSettings:HEADER_SEARCH_PATHS: string " + ofPathToString(p));
		}
	}

	// addLibrary function
	for (auto & f : a->filteredMap["libs"]) {
		for (auto & c : buildConfigs) {
			fs::path p = a->path / f;
			// alert ("-> libs " + p.string(), 96);

			addCommand("Add :objects:" + c + ":buildSettings:OTHER_LDFLAGS: string " + ofPathToString(p));
		}
	}

	for (const fs::path & f : a->filteredMap["frameworks"]) {
		addFramework(a->path / f);
	}

	if (a->addonProperties.count("ADDON_FRAMEWORKS")) {
		for (const auto & f : a->addonProperties["ADDON_FRAMEWORKS"]) {
			addFramework(f);
		}
	}

	const std::map<std::string, std::string> addonToXCode {
		{ "ADDON_CFLAGS", "OTHER_CFLAGS" },
		{ "ADDON_CPPFLAGS", "OTHER_CPLUSPLUSFLAGS" },
		{ "ADDON_LDFLAGS", "OTHER_LDFLAGS" },
		// addDefine
		// { "", "GCC_PREPROCESSOR_DEFINITIONS" },
	};

	for (auto & param : addonToXCode) {
		if (a->addonProperties.count(param.first)) {
			for (const auto & c : buildConfigs) {
				// FIXME: add array here if it doesnt exist. Test with multiple lines
				for (const auto & flag : a->addonProperties[param.first]) {
					addCommand("Add :objects:" + c + ":buildSettings:" + param.second + ": string " + flag);
				}
			}
		}
	}
}

void ofTemplateMacos::addFramework(const fs::path & path) {
	// TODO: Convert this in a function to parse both ADDON_FRAMEWORKS definition in .mk and filesystem frameworks found.
	// void addFramework (const std::string & path);
	alert("		addFramework " + path.string(), 95);

	std::string pathString = path.string();

	bool isRelativeToSDK = false;
	size_t found = pathString.find('/');
	if (found == string::npos) {
		isRelativeToSDK = true;
	}

	fileProperties fp;
	// I had to change this so all frameworks goes to a unique folder called framework. I can change this if it is important
	// fp.absolute = true;
	fp.absolute = false;
	fp.codeSignOnCopy = !isRelativeToSDK;
	fp.copyFilesBuildPhase = !isRelativeToSDK;
	fp.isRelativeToSDK = isRelativeToSDK;
	// fp.frameworksBuildPhase = (target != "ios" && !folder.empty());
	fp.frameworksBuildPhase = target != "ios";

	string UUID;
	if (isRelativeToSDK) {
		fs::path frameworkPath { "System/Library/Frameworks/" + pathString + ".framework" };
		// fs::path frameworkPath { "System/Library/Frameworks/" + pathString + ".framework" };
		UUID = addFile(frameworkPath, "Frameworks", fp);
	} else {
		// std::string folder { "Frameworks" };
		fs::path folder { path.parent_path() };

		fs::path p = path.parent_path();
		fs::path p2 = relative(p, conf.ofPath);

		// alert("-----------");
		// alert(path.string(), 96);
		// alert(p2.string(), 95);
		// alert("-----------");

		UUID = addFile(path, p2, fp);
	}

	if (!isRelativeToSDK) {
		fs::path pathFS { path };
		addCommand("# ----- FRAMEWORK_SEARCH_PATHS");
		string parent { pathFS.parent_path().string() };
		string ext { pathFS.extension().string() };

		for (auto & c : buildConfigs) {
			if (ext == ".framework") {
				addCommand("Add :objects:" + c + ":buildSettings:FRAMEWORK_SEARCH_PATHS: string " + parent);
			}
			if (ext == ".xcframework") {
				addCommand("Add :objects:" + c + ":buildSettings:XCFRAMEWORK_SEARCH_PATHS: string " + parent);
			}
		}
	}
}

void ofTemplateMacos::save() {
	alert("ofTemplateMacos::save()", 92);

	//	debugCommands = false;

	fs::path fileName { conf.projectPath / (conf.projectName + ".xcodeproj/project.pbxproj") };
	bool usePlistBuddy = false;

	if (usePlistBuddy) {
		//	PLISTBUDDY - Mac only
		string command = "/usr/libexec/PlistBuddy " + ofPathToString(fileName);
		string allCommands = "";
		for (auto & c : commands) {
			command += " -c \"" + c + "\"";
			allCommands += c + "\n";
		}
		system(command.c_str());
		// cout << system(command) << endl;
	} else {
		// JSON Block - Multiplatform

		std::ifstream contents(fileName);
		//		std::cout << contents.rdbuf() << std::endl;
		using nlohmann::json;
		json j;
		try {
			j = json::parse(contents);

			// Ugly hack to make nlohmann json work with v 3.11.3
			//			auto dump = j.dump(1, '	');
			//			if (dump[0] == '[') {
			//				j = j[0];
			//			}

		} catch (json::parse_error & ex) {
			std::cerr << "JSON parse error at byte " << ex.byte << std::endl;
			std::cerr << "fileName " << fileName << std::endl;
			std::cerr << contents.rdbuf() << std::endl;
		}

		contents.close();

		for (auto & c : commands) {
			//			alert (c, 31);
			// readable comments enabled now.
			if (c != "" && c[0] != '#') {
				vector<string> cols { ofSplitString(c, " ") };
				string thispath { cols[1] };
				// cout << thispath << endl;
				// stringReplace(thispath, "\:", "\/");

				std::replace(thispath.begin(), thispath.end(), ':', '/');

				// cout << thispath << endl;
				// cout << "----" << endl;
				// ofStringReplace(thispath, ":", "/");

				if (thispath.substr(thispath.length() - 1) != "/") {
					//if (cols[0] == "Set") {
					try {
						json::json_pointer p { json::json_pointer(thispath) };

						if (cols[2] == "string") {
							// find position after find word
							auto stringStart { c.find("string ") + 7 };
							try {
								j[p] = c.substr(stringStart);
							} catch (std::exception & e) {

								std::cerr << "substr " << c.substr(stringStart) << "\n"
										  << "pointer " << p << "\n"
										  << e.what()
										  << std::endl;
								std::exit(1);
							}
							// j[p] = cols[3];
						} else if (cols[2] == "array") {
							try {
								//								j[p] = {};
								j[p] = json::array({});
							} catch (std::exception & e) {
								std::cerr << "array " << e.what() << std::endl;
								std::exit(1);
							}
						}
					} catch (std::exception & e) {
						std::cerr << "json error " << std::endl;
						std::cout << "pointer " << thispath << std::endl;
						std::cerr << e.what() << std::endl;
						std::cerr << "--------------------------------------------------" << std::endl;
						std::exit(1);
					}

				} else {
					thispath = thispath.substr(0, thispath.length() - 1);
					//					cout << thispath << endl;
					json::json_pointer p = json::json_pointer(thispath);
					try {
						// Fixing XCode one item array issue
						if (!j[p].is_array()) {
							//							cout << endl;
							//							alert (c, 31);
							//							cout << "this is not array, creating" << endl;
							//							cout << thispath << endl;
							auto v { j[p] };
							j[p] = json::array();
							if (!v.is_null()) {
								//								cout << "thispath " << thispath << endl;
								j[p].emplace_back(v);
							}
						}
						//						alert (c, 31);
						//						alert ("emplace back " + cols[3] , 32);
						j[p].emplace_back(cols[3]);

					} catch (std::exception & e) {
						std::cerr << "json error " << std::endl;
						std::cerr << e.what() << std::endl;
						std::cerr << thispath << std::endl;
						std::cerr << "-------------------------" << std::endl;
					}
				}
				//				alert("-----", 32);
			}
		}

		std::ofstream jsonFile(fileName);

		// This is not pretty but address some differences in nlohmann json 3.11.2 to 3.11.3
		auto dump = j.dump(1, '	');
		if (dump[0] == '[') {
			dump = j[0].dump(1, '	');
		}

		try {
			jsonFile << dump;
		} catch (std::exception & e) {
			std::cerr << "Error saving json to " << fileName << ": " << e.what() << std::endl;
			;
			// return false;
		} catch (...) {
			std::cerr << "Error saving json to " << fileName << std::endl;
			;
			// return false;
		}
		jsonFile.close();
	}

	//	for (auto & c : commands) cout << c << endl;
	// return true;
}

std::string generateUUID(const string & input) {
	return uuidxx::uuid::Generate().ToString(false);
}

void copyTemplateFile::info() {
	alert("	copyTemplateFile", 96);
	alert("	from " + from.string(), 2);
	alert("	to " + to.string(), 90);
	for (auto & f : findReplaces) {
		if (!empty(f.first)) {
			alert("	└─ Replacing " + f.first + " : " + f.second, 0);
			// std::cout << "	└─ Replacing " << f.first << " : " << f.second << std::endl;
		}
	}
	std::cout << std::endl;
}

bool copyTemplateFile::run() {

	if (fs::exists(from)) {
		info();
		// ofLogVerbose() << "copyTemplateFile from: " << from << " to: " << to;
		// alert("	copyTemplateFile", 92);
		// alert("	from: " + from.string(), 2);
		// alert("	to: " + to.string(), 90);

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
				// alert("	└─ Replacing " + f.first + " : " + f.second, 0);
				// std::cout << "	└─ Replacing " << f.first << " : " << f.second << std::endl;
			}

			for (auto & a : appends) {
				alert("		└─append " + a, 2);
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
					// Remove exists? Remove destination folder?
					if (!fs::exists(to)) {
						fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::update_existing);
					}
				} catch (fs::filesystem_error & e) {
					// catch (const std::exception & e) {
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

void ofTemplateMake::load() {
	alert("ofTemplateMake::load()", 92);

	copyTemplateFiles.push_back({ path / "Makefile",
		conf.projectPath / "Makefile" });

	copyTemplateFiles.push_back({ path / "config.make",
		conf.projectPath / "config.make" });
}

void ofTemplateZed::load() {
	alert("ofTemplateZed::load()", 92);

	// bool ok = ofTemplateMake::load();

	copyTemplateFiles.push_back({ path / "compile_flags.txt",
		conf.projectPath / "compile_flags.txt" });

	copyTemplateFiles.push_back({ path / ".zed",
		conf.projectPath / ".zed" });

	copyTemplateFiles.back().isFolder = true;
}

void ofTemplateZed::save() {
	alert("ofTemplateZed::save()", 92);
	for (auto & a : conf.addons) {
		alert("addon " + a->name, 97);
		// for (auto & f : a->filteredMap) {
		// 	alert(">>>" + f.first);
		// 	for (auto & s : f.second) {
		// 		alert("   " + s.string(), 92);
		// 	}
		// }

		for (auto & f : a->filteredMap["includes"]) {
			std::string inc { "-I" + fs::path(a->path / f).string() };
			copyTemplateFiles[0].appends.emplace_back(inc);
		}
	}
}

void ofTemplateMacos::load() {
	alert("ofTemplateMacos::load()", 92);
	alert("here all load save and replace operations are loaded to memory, but not yet executed.");

	// ALL ABOUT FILES HERE
	// auto projectName = conf.projectPath.filename().string();
	fs::path xcodeProject { conf.projectPath / (conf.projectName + ".xcodeproj") };
	// cout << xcodeProject << endl;

	try {
		fs::create_directories(xcodeProject);
	} catch (const std::exception & e) {
		std::cerr << "Error creating directories: " << e.what() << std::endl;
	}

	std::pair<string, string> rootReplacements;

	// Just replace ofPath if it is not default relative to project
	if (!fs::equivalent(conf.ofPath, "../../..")) {
		rootReplacements = { "../../..", conf.ofPath.string() };
	}

	copyTemplateFiles.push_back({ path / "emptyExample.xcodeproj" / "project.pbxproj",
		xcodeProject / "project.pbxproj",
		{ { "emptyExample", conf.projectName },
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
	auto schemeFolder = conf.projectPath / (conf.projectName + ".xcodeproj") / "xcshareddata/xcschemes";

	if (fs::exists(schemeFolder)) {
		fs::remove_all(schemeFolder);
	}
	fs::create_directories(schemeFolder);

	if (target == "osx" || target == "macos") {
		for (auto & f : { "Release", "Debug" }) {
			copyTemplateFiles.push_back({ path / ("emptyExample.xcodeproj/xcshareddata/xcschemes/emptyExample " + string(f) + ".xcscheme"),
				schemeFolder / (conf.projectName + " " + f + ".xcscheme"),
				{ { "emptyExample", conf.projectName } } });
		}

		copyTemplateFiles.push_back({ conf.projectPath / (conf.projectName + ".xcodeproj/project.xcworkspace"),
			path / "emptyExample.xcodeproj/project.xcworkspace" });
	} else {

		// MARK:- IOS sector;
		copyTemplateFiles.push_back({ path / "emptyExample.xcodeproj/xcshareddata/xcschemes/emptyExample.xcscheme",
			schemeFolder / (conf.projectName + ".xcscheme"),
			{ { "emptyExample", conf.projectName } } });
	}

	// END ALL ABOUT FILES
	//
	addCommand("Add :_OFProjectGeneratorVersion string " + getPGVersion());

	// RENAME projectName inside templates
	// FIXME: review BUILT_PRODUCTS_DIR
	//

	folderUUID = {
		// { "src", "E4B69E1C0A3A1BDC003C02F2" },
		{ "addons", "BB4B014C10F69532006C3DED" },
		{ "openFrameworks", "191EF70929D778A400F35F26" },
		{ "", "E4B69B4A0A3A1720003C02F2" }
		// { "localAddons",	"6948EE371B920CB800B5AC1A" },
	};

	if (target == "ios") {
		folderUUID = {
			// { "src", "E4D8936A11527B74007E1F53" },
			{ "addons", "BB16F26B0F2B646B00518274" },
			{ "", "29B97314FDCFA39411CA2CEA" },
			// { "Frameworks", "901808C02053638E004A7774" }
			// { "localAddons", 	"6948EE371B920CB800B5AC1A" },
		};

		uuid["buildConfigurationList"] = "1D6058900D05DD3D006BFB54";
		uuid["buildActionMask"] = "1D60588E0D05DD3D006BFB54";
		// uuid["projRoot"] = "29B97314FDCFA39411CA2CEA";
		uuid["frameworks"] = "1DF5F4E00D08C38300B7A737";
		uuid["afterPhase"] = "928F60851B6710B200E2D791";
		uuid["buildPhases"] = "9255DD331112741900D6945E";

		// things specific for ios
		uuid["resources"] = "BB24DD8F10DA77E000E9C588";
		uuid["buildPhaseResources"] = "1D60588D0D05DD3D006BFB54";
		uuid["mediaAssets"] = "9936F60E1BFA4DEE00891288";

		buildConfigurations[0] = "1D6058940D05DD3E006BFB54"; // iOS Debug
		buildConfigurations[1] = "1D6058950D05DD3E006BFB54"; // iOS Release
		buildConfigurations[2] = "C01FCF4F08A954540054247B"; // iOS Debug
		buildConfigurations[3] = "C01FCF5008A954540054247B"; // iOS Release

		buildConfigs[0] = "1D6058940D05DD3E006BFB54"; // iOS Debug
		buildConfigs[1] = "1D6058950D05DD3E006BFB54"; // iOS Release
	}

	addCommand("Set :objects:" + uuid["buildConfigurationList"] + ":name " + conf.projectName);

	// Just OSX here, debug app naming.
	if (target == "osx" || target == "macos") {
		// TODO: Hardcode to variable
		// FIXME: Debug needed in name?
		addCommand("Set :objects:E4B69B5B0A3A1756003C02F2:path " + conf.projectName + "Debug.app");
	}

	fileProperties fp;
	//	fp.isGroupWithoutFolder = true;
	//	addFile("additionalSources", "", fp);
	//	fp.isGroupWithoutFolder = false;
	//	addFile("openFrameworks-Info.plist", "", fp);
	//	addFile("of.entitlements", "", fp);
	//	addFile("Project.xcconfig", "", fp);
	if (fs::exists(conf.projectPath / "App.xcconfig")) {
		addFile("App.xcconfig", "", fp);
	}
	fp.absolute = true;
	//	addFile("../../../libs/openframeworks", "", fp);
	addFile(fs::path { "bin" } / "data", "", fp);
	// addFile(conf.ofPath / "libs" / "macos" / "include", "", fp);
	// add sources

	// for (auto & f :)
	// add headers files
}

string ofTemplateMacos::getFolderUUID(const fs::path & folder, fs::path base) {
	// alert("xcodeProject::getFolderUUID " + folder.string() + " base:" + base.string(), 95); //+" : isfolder="+ofToString(isFolder)+" : base="+ base.string());
	auto fullPathFolder = folder;

	// If folder UUID exists just return it.
	if (folderUUID.find(fullPathFolder) != folderUUID.end()) { // NOT FOUND
		return folderUUID[fullPathFolder];
	} else {
		// in this case it is not found, so it creates UUID for the entire path
		vector<fs::path> folders = std::vector(folder.begin(), folder.end());
		string lastFolderUUID = folderUUID[""]; // root folder uuid
		string lastFolder = "";

		if (folders.size()) {
			// Iterating every folder from full path
			for (std::size_t a = 0; a < folders.size(); a++) {
				fs::path fullPath { "" };

				std::vector<fs::path> joinFolders;
				joinFolders.reserve(a + 1); // Reserve / avoid reallocations

				for (std::size_t i = 0; i <= a; ++i) {
					joinFolders.push_back(folders[i]);
				}

				for (const auto & j : joinFolders) {
					fullPath /= j;
				}

				// Query if partial path is already stored. if not execute this following block
				if (folderUUID.find(fullPath) != folderUUID.end()) {
					lastFolderUUID = folderUUID[fullPath];
					lastFolder = folderFromUUID[lastFolderUUID].string();
				}

				else {
					// CREATING A NEW FOLDER
					string thisUUID = generateUUID(fullPath);
					folderUUID[fullPath] = thisUUID;
					folderFromUUID[thisUUID] = fullPath;

					string folderName = ofPathToString(folders[a]);
					addCommand("Add :objects:" + thisUUID + ":name string " + folderName);
					addCommand("Add :objects:" + thisUUID + ":isa string PBXGroup");

					// Assure we are on root
					bool folderSet = false;

					if (lastFolderUUID == folderUUID[""]) {
						if (folderName == "Frameworks") {
							addCommand("Add :objects:" + thisUUID + ":path string System/Library/Frameworks");
							addCommand("Add :objects:" + thisUUID + ":sourceTree string SDKROOT");
							folderSet = true;
						}
						if (folderName == "external_sources" || folderName == "local_addons") {
							addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");
							addCommand("Add :objects:" + thisUUID + ":path string " + folderName);
							folderSet = true;
						}
					}

					if (!folderSet) {
						// addCommand("Add :objects:" + thisUUID + ":sourceTree string SOURCE_ROOT");
						addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");
						addCommand("Add :objects:" + thisUUID + ":path string " + folderName);
					}

					// TODO: REMOVE
					// fs::path checkPath { folderFromUUID[lastFolderUUID] / folderFromUUID[thisUUID] };
					// if (fs::exists(checkPath)) {
					// 	alert("yes " + checkPath.string(), 95);
					// } else {
					// 	alert("no " + checkPath.string(), 97);
					// }

					// if (folderName == "external_sources" || folderName == "local_addons") {

					// 	//						addCommand("Add :objects:"+thisUUID+":sourceTree string SOURCE_ROOT");
					// 	//						addCommand("Add :objects:"+thisUUID+":path string ");
					// 	addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");

					// 	folderSet = true;
					// } else {
					// 	if (lastFolderUUID == folderUUID[""]) { //} ||
					// 		//                            lastFolder == "external_sources" || lastFolder == "local_addons") { //

					// 		// fs::path base2 { "" };
					// 		// size_t diff = folders.size() - (a + 1);
					// 		// for (size_t x = 0; x < diff; x++) {
					// 		// 	base2 = base2.parent_path();
					// 		// }

					// 		//                            alert ("external_sources base = " + ofPathToString(base2) + " UUID: " + thisUUID, 33);
					// 		addCommand("Add :objects:" + thisUUID + ":sourceTree string SOURCE_ROOT");
					// 		addCommand("Add :objects:" + thisUUID + ":path string " + folderName);
					// 		// alert (commands.back(), 95);
					// 		// addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(base2));
					// 		// alert ("xxxx: " + ofPathToString(base2), 33);
					// 		// alert (commands.back(), 95);

					// 		folderSet = true;
					// 	} else if (lastFolder == "external_sources" || lastFolder == "local_addons") { //
					// 		addCommand("Add :objects:" + thisUUID + ":sourceTree string SOURCE_ROOT");
					// 		//                            alert ("xxxx " + lastFolder + "  " + ofPathToString(base) + " UUID: " + thisUUID, 33);
					// 		addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(getPathTo(base, folderName)));
					// 		folderSet = true;
					// 	} else {
					// 		addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");
					// 		//							fs::path addonFolder { fs::path(fullPath).filename() };
					// 		addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(fullPath.filename()));
					// 		folderSet = true;
					// 	}
					// }

					addCommand("Add :objects:" + thisUUID + ":children array");

					// if (!bFolderPathSet) {
					// 	if (folder.begin()->string() == "addons" || folder.begin()->string() == "src") { //} || folder.begin()->string() == "local_addons") {
					// 		addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");
					// 		//						fs::path addonFolder { fs::path(fullPath).filename() };
					// 		addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(fullPath.filename()));
					// 		// alert ("group " + folder.string() + " : " + base.string() + " : " + addonFolder.string(), 32);
					// 	} else {
					// 		addCommand("Add :objects:" + thisUUID + ":sourceTree string SOURCE_ROOT");
					// 	}
					// }

					// Add this new folder to its parent, folderUUID[""] if root
					addCommand("Add :objects:" + lastFolderUUID + ":children: string " + thisUUID);

					// keep this UUID as parent for the next folder.
					lastFolderUUID = thisUUID;
					lastFolder = folderName;
				}
			}
		}
		return lastFolderUUID;
	}
}

struct fileJson {
	fs::path fileName;
	json data;

	// only works for workspace
	void addPath(fs::path folder) {
		std::string path = folder.is_absolute() ? folder.string() : "${workspaceRoot}/../" + folder.string();
		json object;
		object["path"] = path;
		json::json_pointer p = json::json_pointer("/folders");
		data[p].emplace_back(object);
	}

	void addToArray(string pointer, fs::path value) {
		json::json_pointer p = json::json_pointer(pointer);
		if (!data[p].is_array()) {
			data[p] = json::array();
		}

		std::string path = fs::path(value).is_absolute() ? value.string() : "${workspaceRoot}/" + value.string();
		data[p].emplace_back(path);
	}

	void load() {
		if (!fs::exists(fileName)) {
			std::cerr << "JSON file not found " << fileName.string() << std::endl;
			return;
		}

		std::ifstream ifs(fileName);
		try {
			data = json::parse(ifs);
		} catch (json::parse_error & ex) {
			std::cerr << "JSON parse error at byte" << ex.byte << std::endl;
			std::cerr << "fileName" << fileName.string() << std::endl;
		}
	}

	void save() {
		//		alert ("saving now " + fileName.string(), 33);
		//		std::cout << data.dump(1, '\t') << std::endl;
		std::ofstream jsonFile(fileName);
		try {
			jsonFile << data.dump(1, '\t');
		} catch (std::exception & e) {
			std::cerr << "Error saving json to " << fileName.string() << ": " << e.what() << std::endl;
		}
	}
};

fileJson workspace;
fileJson cppProperties;

void ofTemplateVSCode::load() {
	alert("ofTemplateVSCode::load()", 92);

	// bool ok = ofTemplateMake::load();

	copyTemplateFiles.push_back({ path / ".vscode",
		conf.projectPath / ".vscode" });
	copyTemplateFiles.back().isFolder = true;

	workspace.fileName = conf.projectPath / (conf.projectName + ".code-workspace");
	cppProperties.fileName = conf.projectPath / ".vscode/c_cpp_properties.json";

	copyTemplateFiles.push_back({ path / "emptyExample.code-workspace",
		conf.projectPath / workspace.fileName });

	// copyTemplateFiles.push_back({ path / "compile_flags.txt",
	// conf.projectPath / "compile_flags.txt" });

	workspace.load();
	cppProperties.load();

	for (auto & a : conf.addons) {
		alert("parsing addon " + a->name, 97);
		for (auto & f : a->filteredMap) {
			alert(">>>" + f.first);
			for (auto & s : f.second) {
				alert("   " + s.string(), 92);
			}
		}

		for (auto & f : a->filteredMap["includes"]) {
			workspace.addPath(a->path);
		}

		// std::string inc { "-I" + fs::path(a->path / f).string() };
		// copyTemplateFiles[0].appends.emplace_back(inc);
		// }
	}
}

void ofTemplateVSCode::save() {
	alert("ofTemplateVSCode::save()", 92);
	workspace.data["openFrameworksProjectGeneratorVersion"] = getPGVersion();
	workspace.save();
	cppProperties.save();
}

void ofTemplateMake::save() {
	alert("ofTemplateMake::save()", 92);
	if (conf.addons.size()) {
		alert("    saving addons.make", 2);
		fs::path fileName { conf.projectPath / "addons.make" };
		std::ofstream addonsMake(fileName);
		for (auto & a : conf.addons) {
			if (!a->isProject) {
				addonsMake << a->name << std::endl;
			}
		}
		addonsMake.close();
	}
}
