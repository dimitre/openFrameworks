#include "templates.h"
#include "addons.h"
#include "uuidxx.h"
#include <fstream> //in utils
#include <nlohmann/json.hpp>

using nlohmann::json;

std::string generateUUID(const std::string & input) {
	return uuidxx::uuid::Generate().ToString(false);
}

void copyTemplateFile::info() {
	std::cout << std::endl;
	alert("	copyTemplateFile", 96);
	alert("	from " + from.string(), 2);
	alert("	to " + to.string(), 90);
	for (auto & f : findReplaces) {
		if (!empty(f.first)) {
			alert("	└─ Replacing " + f.first + " : " + f.second, 94);
			// std::cout << "	└─ Replacing " << f.first << " : " << f.second << std::endl;
		}
	}
}

void copyTemplateFile::load() {
	if (fs::exists(from)) {
		isLoaded = true;
		std::ifstream fileFrom(from);
		// std::string contents((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
		contents = std::string((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
		fileFrom.close();
	}
}

bool copyTemplateFile::run() {
	info();
	if (fs::exists(from)) {

		if (findReplaces.size() || appends.size()) { // || transform != nullptr
			// Load file, replace contents, append data to content, make transformation and then write to destination.

			if (!isLoaded) {
				load();
			}
			// std::ifstream fileFrom(from);
			// // std::string contents((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
			// contents = std::string((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
			// fileFrom.close();

			// if (transform != nullptr) {
			// 	(*transform)(contents);
			// }
			// alert("AFTER TRANSFORM", 95);
			// cout << contents << endl;

			for (auto & f : findReplaces) {
				// Avoid processing empty pairs
				if (empty(f.first) && empty(f.second)) {
					continue;
				}
				replaceAll(contents, f.first, f.second);
			}

			for (auto & a : appends) {
				alert("	└─append " + a, 94);
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
				// Remove exists? Remove destination folder?
				if (fs::exists(to)) {
					try {
						fs::remove_all(to);
					} catch (fs::filesystem_error & e) {
						std::cerr << "Error removing template folder: " << to << " " << e.what() << std::endl;
						return false;
					}
				}
				try {
					fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::update_existing);
				} catch (fs::filesystem_error & e) {
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
	}

	else {
		alert("	input file not found " + from.string(), 91);
		return false;
	}
	return true;
}

std::string ofTemplateMacos::addFile(const fs::path & path, const fs::path & folder, const fileProperties & fp) {
	std::string UUID { "" };
	//	alert("xc::addFile " + path.string() + " :folder:" + folder.string(), 31);

	fs::path ext { path.extension() };
	{
		std::string fileType = extensionToFileType[ext];

		if (empty(fileType)) {
			if (fs::is_directory(path) || fp.isGroupWithoutFolder) {
				fileType = "folder";
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

		// if (!empty(fileType)) {
		// addCommand("Add :objects:" + UUID + ":lastKnownFileType string " + fileType);
		// }

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
			if (!fp.isGroupWithoutFolder) {
				addCommand("Add :objects:" + UUID + ":path string " + ofPathToString(path.filename()));
			}
			addCommand("Add :objects:" + UUID + ":sourceTree string <group>");
		}

		// Eventually remove isFolder and base parameter
		std::string folderUUID { getFolderUUID(folder, path) };

		// addCommand("# ---- addFileToFolder UUID : " + ofPathToString(folder));
		addCommand("Add :objects:" + folderUUID + ":children: string " + UUID);

		std::string buildUUID { generateUUID(ofPathToString(path) + "-build") };
		// If any other option is true, add buildUUID entries.
		if (
			fp.addToBuildPhase || fp.codeSignOnCopy || fp.copyFilesBuildPhase || fp.addToBuildResource || fp.addToResources
			//|| fp.frameworksBuildPhase ~ I've just removed this one, favoring -InFrameworks
		) {
			addCommand("# ---- addFileToBuildUUID " + buildUUID);
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
			if (ext == ".framework") { // || fileType == ".xcframework"
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

		if (fp.linkBinaryWithLibraries) { // Link Binary With Libraries
			// alert("fp.linkBinaryWithLibraries " + path.string(), 35);
			auto tempUUID = generateUUID(ofPathToString(path) + "-InFrameworks");
			addCommand("Add :objects:" + tempUUID + ":fileRef string " + UUID);
			addCommand("Add :objects:" + tempUUID + ":isa string PBXBuildFile");

			addCommand("# --- linkBinaryWithLibraries");
			addCommand("Add :objects:" + uuid["linkBinaryWithLibraries"] + ":files: string " + tempUUID);
		}

		// if (path.extension() == ".framework") {
		// 	addCommand("# ---- Frameworks Folder " + UUID);
		// 	addCommand("Add :objects:901808C02053638E004A7774:children: string " + UUID);

		// 	addCommand("# ---- PBXFrameworksBuildPhase " + buildUUID);
		// 	addCommand("Add :objects:1D60588F0D05DD3D006BFB54:files: string " + buildUUID);
		// }
	}
	return UUID;
}

void ofTemplateMacos::addSrc(const fs::path & srcFile, const fs::path & folder) {
	// alert ("xcodeProject::addSrc " + ofPathToString(srcFile) + " : " + ofPathToString(folder), 31);

	std::string ext { srcFile.extension().string() };

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

	if (ext == ".h" || ext == ".hpp" || ext == ".tcc" || ext == ".inl" || ext == ".in") {
		fp.addToBuildPhase = false;
	} else if (ext == ".xib") {
		fp.addToBuildPhase = false;
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".metal") {
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".entitlements") {
		// fixme, needed?
		// fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".info") {
		// fp.addToBuildResource = true;
		fp.addToResources = true;
	}

	// FROM PG Latest
	else if (ext == ".xcassets") {
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".modulemap") {
		fp.addToBuildPhase = false;
	} else if (ext == ".bundle") {
		fp.addToBuildResource = true;
		fp.addToResources = true;
	} else if (ext == ".tbd") {
		fp.linkBinaryWithLibraries = true;
	}

	else if (ext == ".swift") {
		fp.addToBuildPhase = true;
		for (auto & c : buildConfigs) {
			addCommand("Add :objects:" + c + ":buildSettings:OTHER_SWIFT_FLAGS: string -cxx-interoperability-mode=swift-5.9");
			// mark all Swift files as C++ interop available :)
		}
	}

	if (target == "ios") {
		fp.addToBuildPhase = true;
		fp.addToResources = true;
	}

	std::string UUID {
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
			for (const auto & s : ofSplitString(f, " ")) {
				addFramework(s);
			}
		}
	}

	const std::map<std::string, std::string> addonToXCode {
		{ "ADDON_CFLAGS", "OTHER_CFLAGS" },
		{ "ADDON_CPPFLAGS", "OTHER_CPLUSPLUSFLAGS" },
		{ "ADDON_LDFLAGS", "OTHER_LDFLAGS" },
		{ "ADDON_DEFINES", "GCC_PREPROCESSOR_DEFINITIONS" },
	};

	for (auto & param : addonToXCode) {
		if (a->addonProperties.count(param.first)) {
			for (const auto & c : buildConfigs) {
				// FIXME: add array here if it doesnt exist. Test with multiple lines
				for (const auto & flag : a->addonProperties[param.first]) {
					addCommand("Add :objects:" + c + ":buildSettings:" + param.second + ": string " + flag);
					// alert(commands.back(), 95);
				}
			}
		}
	}
}

void ofTemplateMacos::addFramework(const fs::path & path) {
	// TODO: Convert this in a function to parse both ADDON_FRAMEWORKS definition in .mk and filesystem frameworks found.
	// void addFramework (const std::string & path);
	alert(" addFramework " + path.string(), 95);

	std::string pathString = path.string();

	bool isRelativeToSDK = false;
	size_t found = pathString.find('/');
	if (found == std::string::npos) {
		isRelativeToSDK = true;
	}

	fileProperties fp;
	// I had to change this so all frameworks goes to a unique folder called framework. I can change this if it is important
	// fp.absolute = true;
	fp.absolute = false;
	fp.codeSignOnCopy = !isRelativeToSDK;
	fp.copyFilesBuildPhase = !isRelativeToSDK;
	fp.isRelativeToSDK = isRelativeToSDK;
	// fp.frameworksBuildPhase = target != "ios";
	// fp.linkBinaryWithLibraries = !	fp.isRelativeToSDK && path.extension().string() == ".framework";
	// fp.linkBinaryWithLibraries = path.extension().string() == ".framework";
	fp.linkBinaryWithLibraries = path.extension().string() != ".xcframework";

	std::string UUID { "" };
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
		addCommand("# ----- FRAMEWORK_SEARCH_PATHS");
		std::string parent { path.parent_path().string() };
		std::string ext { path.extension().string() };

		// alert("path " + path.string(), 94);
		// alert("parent " + parent, 94);

		for (auto & c : buildConfigs) {
			if (ext == ".framework") {
				fp.linkBinaryWithLibraries = true;
				// addCommand("Add :objects:" + c + ":buildSettings:FRAMEWORK_SEARCH_PATHS array");
				addCommand("Add :objects:" + c + ":buildSettings:FRAMEWORK_SEARCH_PATHS: string " + parent);
			}
			if (ext == ".xcframework") {
				// addCommand("Add :objects:" + c + ":buildSettings:XCFRAMEWORK_SEARCH_PATHS array");
				addCommand("Add :objects:" + c + ":buildSettings:XCFRAMEWORK_SEARCH_PATHS: string " + parent);
			}
		}
	}
}

void ofTemplateMacos::edit(std::string & str) {
	using nlohmann::json;
	json j;
	try {
		j = json::parse(str);
	} catch (json::parse_error & e) {
		std::cerr << "JSON parse error at byte " << e.byte << std::endl;
		// std::cerr << contents.rdbuf() << std::endl;
	}

	for (auto & c : commands) {
		if (c == "" || c[0] == '#') {
			continue;
		}

		std::vector<std::string> cols { ofSplitString(c, " ") };
		std::string thispath { cols[1] };

		std::replace(thispath.begin(), thispath.end(), ':', '/');

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
					// alert("ARRAYYY " + cols[1] + ":" + cols[3], 94);

				} else {
					alert("WOWW " + cols[2], 94);
					std::exit(1);
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
				//
				// bool notyet = false;
				if (!j[p].is_array()) {
					// alert("this path is not array yet " + thispath, 96);
					j[p] = json::array();
					// notyet = true;
					// auto v { j[p] };
					// if (!v.is_null()) {
					// j[p].emplace_back(v);
					// }
				}

				std::string val = cols[3];
				if (cols[2] == "string") {
					// find position after find word
					auto stringStart { c.find("string ") + 7 };
					val = c.substr(stringStart);
					// try {
					// 	j[p].emplace_back(c.substr(stringStart));
					// }
				}
				j[p].emplace_back(val);

				// if (notyet) {
				// 	alert(thispath, 95);
				// 	alert(val, 96);
				// }
				// alert("OWW2 " + val, 96);

			} catch (std::exception & e) {
				std::cerr << "json error " << std::endl;
				std::cerr << e.what() << std::endl;
				std::cerr << thispath << std::endl;
				std::cerr << "-------------------------" << std::endl;
			}
		}
	}

	// This is not pretty but address some differences in nlohmann json 3.11.2 to 3.11.3
	auto dump = j.dump(1, '	');
	if (dump[0] == '[') {
		dump = j[0].dump(1, '	');
	}

	str = dump;
	// cout << dump << endl;
}

void ofTemplateMake::load() {
	alert("ofTemplateMake::load()", 92);

	copyTemplateFiles.push_back({ path / "config.make",
		conf.projectPath / "config.make" });

	for (auto & l : conf.nodeToStrings("make")) {
		copyTemplateFiles.back().appends.emplace_back(l);
		// alert(l, 35);
	}

	copyTemplateFiles.push_back({ path / "Makefile",
		conf.projectPath / "Makefile" });
}

void ofTemplateZed::load() {
	alert("ofTemplateZed::load()", 92);

	// bool ok = ofTemplateMake::load();

	copyTemplateFiles.push_back({ path / "compile_flags.txt",
		conf.projectPath / "compile_flags.txt" });

	// copyTemplateFiles.push_back({ path / ".zed",
	// 	conf.projectPath / ".zed" });
	// copyTemplateFiles.back().isFolder = true;
	fs::path folder { conf.projectPath / ".zed" };
	if (!fs::exists(folder)) {
		fs::create_directory(folder);
	}

	copyTemplateFiles.push_back({ path / ".zed/keymap.json",
		conf.projectPath / ".zed/keymap.json" });
	copyTemplateFiles.push_back({ path / ".zed/settings.json",
		conf.projectPath / ".zed/settings.json" });
	copyTemplateFiles.push_back({ path / ".zed/tasks.json",
		conf.projectPath / ".zed/tasks.json" });

	// copyTemplateFiles.push_back({ path / "emptyExample.xcodeproj" / "project.pbxproj",
	// 	xcodeProject / "project.pbxproj",
	// 	{ { "emptyExample", conf.projectName },
	// 		rootReplacements } });

	copyTemplateFiles.push_back({ path / ".zed/debug.json",
		conf.projectPath / ".zed/debug.json",
		{ { "$PROJECT_NAME", conf.projectName } } });
}

void ofTemplateZed::save() {
	alert("ofTemplateZed::save()", 92);
	for (auto & a : conf.addons) {
		// alert("ofTemplateZed::save addon " + a->name, 95);
		// alert("ofTemplateZed::save filesMap includes " + a->name, 95);

		// for (auto & f : a->filesMap["includes"]) {
		//   cout << f << endl;
		// }
		// alert("ofTemplateZed::save filteredMap includes " + a->name, 95);

		// for (auto & f : a->filteredMap["includes"]) {
		//   cout << f << endl;
		// }
		for (auto & f : a->filteredMap["includes"]) {
			std::string inc { "-I" + fs::path(a->path / f).string() };
			copyTemplateFiles[0].appends.emplace_back(inc);
		}
	}
}

void ofTemplateMacos::load() {
	alert("ofTemplateMacos::load()", 92);
	alert("	here all load save and replace operations are loaded to memory, but not yet executed.", 90);

	// ALL ABOUT FILES HERE
	// auto projectName = conf.projectPath.filename().string();
	fs::path xcodeProject { conf.projectPath / (conf.projectName + ".xcodeproj") };
	// cout << xcodeProject << endl;

	try {
		fs::create_directories(xcodeProject);
	} catch (const std::exception & e) {
		std::cerr << "Error creating directories: " << e.what() << std::endl;
	}

	std::pair<std::string, std::string> rootReplacements;

	// Just replace ofPath if it is not default relative to project
	if (!fs::equivalent(conf.ofPath, "../../..")) {
		rootReplacements = { "../../..", conf.ofPath.string() };
	}

	copyTemplateFiles.push_back({ path / "Project.xcconfig",
		conf.projectPath / "Project.xcconfig",
		{ rootReplacements } });

	copyTemplateFiles.push_back({ path / "emptyExample.xcodeproj" / "project.pbxproj",
		xcodeProject / "project.pbxproj",
		{ { "emptyExample", conf.projectName },
			rootReplacements } });
	// We load this because it will edit json in memory, save later.
	copyTemplateFiles.back().load();

	// copyTemplateFiles.back().transform = trans;

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

	if (fs::exists(path / "mediaAssets")) {
		copyTemplateFiles.push_back({ path / "mediaAssets", conf.projectPath / "mediaAssets" });
		copyTemplateFiles.back().isFolder = true;
	}

	// Equivalent to SaveScheme in projectGenerator
	//
	auto schemeFolder = conf.projectPath / (conf.projectName + ".xcodeproj") / "xcshareddata/xcschemes";

	if (fs::exists(schemeFolder)) {
		fs::remove_all(schemeFolder);
	}
	fs::create_directories(schemeFolder);

	if (target == "osx" || target == "macos") {
		for (auto & f : { "Release", "Debug" }) {
			copyTemplateFiles.push_back({ path / ("emptyExample.xcodeproj/xcshareddata/xcschemes/emptyExample " + std::string(f) + ".xcscheme"),
				schemeFolder / (conf.projectName + " " + f + ".xcscheme"),
				{ { "emptyExample", conf.projectName } } });
		}

		copyTemplateFiles.push_back({
			path / "emptyExample.xcodeproj/project.xcworkspace",
			conf.projectPath / (conf.projectName + ".xcodeproj/project.xcworkspace"),
		});
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

		// buildConfigurations[0] = "1D6058940D05DD3E006BFB54"; // iOS Debug
		// buildConfigurations[1] = "1D6058950D05DD3E006BFB54"; // iOS Release
		// buildConfigurations[2] = "C01FCF4F08A954540054247B"; // iOS Debug
		// buildConfigurations[3] = "C01FCF5008A954540054247B"; // iOS Release

		buildConfigs[0] = "1D6058940D05DD3E006BFB54"; // iOS Debug
		buildConfigs[1] = "1D6058950D05DD3E006BFB54"; // iOS Release
		// buildConfigs[2] = "C01FCF4F08A954540054247B"; // iOS Debug
		// buildConfigs[3] = "C01FCF5008A954540054247B"; // iOS Release

		// buildConfigs[0] = "1D6058940D05DD3E006BFB54"; // iOS Debug
		// buildConfigs[1] = "1D6058950D05DD3E006BFB54"; // iOS Release
	}

	addCommand("Set :objects:" + uuid["buildConfigurationList"] + ":name string " + conf.projectName);

	// Just OSX here, debug app naming.
	if (target == "osx" || target == "macos") {
		// TODO: Hardcode to variable
		// FIXME: Debug needed in name?
		addCommand("Set :objects:E4B69B5B0A3A1756003C02F2:path string " + conf.projectName + "Debug.app");

		if (!ofIsPathInPath(fs::current_path(), conf.ofPath)) {
			addCommand("Set :objects:" + folderUUID["openFrameworks"] + ":path string " + conf.ofPath.string() + "/libs/openFrameworks");
			addCommand("Set :objects:" + folderUUID["openFrameworks"] + ":sourceTree string <absolute>");

			addCommand("Set :objects:" + folderUUID["addons"] + ":path string " + conf.ofPath.string() + "/addons");
			addCommand("Set :objects:" + folderUUID["addons"] + ":sourceTree string <absolute>");
		}
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

	if (fs::exists("bin/data")) {
		addFile(fs::path { "bin" } / "data", "", fp);
	}

	for (auto & path : conf.additionalSources) {
		// fs::path filename = a.filename();
		//
		// // if is directory
		//    	string UUID { generateUUID(path) };
		// addCommand("Add :objects:" + UUID + ":sourceTree string SOURCE_ROOT");
		// addCommand("Add :objects:" + UUID + ":isa string PBXGroup");
		// addCommand("Add :objects:" + UUID + ":fileEncoding string 4");
		// addCommand("Add :objects:" + UUID + ":name string " + ofPathToString(path.filename()));
		// addCommand("Add :objects:" + UUID + ":path string " + ofPathToString(path));
		// addCommand("Add :objects:" + UUID + ":sourceTree string <group>");
		fp.addToBuildPhase = true;
		addFile(path, "", fp);
	}

	// Just testing.
	// fp.isSrc = false;
	// fp.isGroupWithoutFolder = true;
	// addFile("external_sources", "", fp);
	//
	//
	// addFile(conf.ofPath / "libs" / "macos" / "include", "", fp);
	// add sources

	// for (auto & f :)
	// add headers files
}

std::string ofTemplateMacos::getFolderUUID(const fs::path & folder, fs::path base) {
	// alert("xcodeProject::getFolderUUID " + folder.string() + " base:" + base.string(), 95); //+" : isfolder="+ofToString(isFolder)+" : base="+ base.string());
	auto fullPathFolder = folder;

	// If folder UUID exists just return it.
	if (folderUUID.find(fullPathFolder) != folderUUID.end()) { // NOT FOUND
		return folderUUID[fullPathFolder];
	} else {
		// in this case it is not found, so it creates UUID for the entire path
		std::vector<fs::path> folders = std::vector(folder.begin(), folder.end());
		std::string lastFolderUUID = folderUUID[""]; // root folder uuid
		std::string lastFolder = "";

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

					addCommand("Add :objects:" + thisUUID + ":children array");

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

	workspace.load();
	cppProperties.load();

	for (auto & a : conf.addons) {
		// alert("parsing addon " + a->name, 97);
		for (auto & f : a->filteredMap) {
			// alert(">>>" + f.first);
			for (auto & s : f.second) {
				alert("   " + s.string(), 92);
			}
		}

		for (auto & f : a->filteredMap["includes"]) {
			workspace.addPath(a->path);
		}
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
		alert(" saving addons.make", 2);
		fs::path fileName { conf.projectPath / "addons.make" };
		std::ofstream addonsMake(fileName);
		for (auto & a : conf.addons) {
			if (!a->isProject) {
				addonsMake << a->name << std::endl;
			}
		}
		addonsMake.close();
	}

	if (conf.defines.size() || conf.additionalSources.size()) {
		std::string allDefines { "# Generated by " + getPGVersion() };
		if (conf.defines.size()) {
			// alert("PROJECT_DEFINES", 95);
			allDefines += "\nPROJECT_DEFINES =";
			for (auto & d : conf.defines) {
				allDefines += " " + d;
			}
		}
		if (conf.additionalSources.size()) {
			allDefines += "\nPROJECT_EXTERNAL_SOURCE_PATHS =";
			for (auto & d : conf.additionalSources) {
				allDefines += " " + d.string();
			}
		}
		copyTemplateFiles[0].appends.emplace_back(allDefines);
	}
}

void ofTemplateMacos::save() {
	alert("ofTemplateMacos::save()", 92);

	// This will edit json file in memory. it reads from template, edits and save with the new name
	// touching HD a lot less than before
	//
	// alert ("BEFORE EDIT", 95);
	// cout << copyTemplateFiles[1].contents.size() << endl;
	edit(copyTemplateFiles[1].contents);
	// alert ("AFTER EDIT", 95);
	// cout << copyTemplateFiles[1].contents.size() << endl;

	if (conf.defines.size()) {
		std::string allDefines { "// Generated by " + getPGVersion() };
		allDefines += "\nGCC_PREPROCESSOR_DEFINITIONS=$(inherited)";
		// std::string allDefines = "GCC_PREPROCESSOR_DEFINITIONS=$(inherited)";
		for (auto & d : conf.defines) {
			allDefines += " " + d;
		}
		copyTemplateFiles[0].appends.emplace_back(allDefines);
	}
	//	for (auto & c : commands) cout << c << endl;
	// return true;
}



void ofTemplateVisualStudio::addAddon(ofAddon * a) {

#ifdef PORT
   	ofLogVerbose("visualStudioProject::") << "Adding addon: [" << addon.name << "]";
	// Handle additional vcxproj files in the addon
	fs::path additionalFolder = addon.addonPath / (addon.name + "Lib");
	if (fs::exists(additionalFolder)) {
		for (const auto &entry : fs::directory_iterator(additionalFolder)) {
			auto f = entry.path();
			if (f.extension() == ".vcxproj") {
				additionalvcxproj.emplace_back(f);
			}
		}
	}
    #endif

	for (auto & f : a->filteredMap["includes"]) {
		fs::path p = a->path / f;
		addInclude(p);
		// alert ("->" + p.string(), 95);
		// addCommand("Add :objects:" + c + ":buildSettings:HEADER_SEARCH_PATHS: string " + ofPathToString(p));
	}
}
