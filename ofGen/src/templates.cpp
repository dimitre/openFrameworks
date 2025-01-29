#include "templates.h"
#include "addons.h"
#include "uuidxx.h"

void ofTemplateMacos::addAddon(ofAddon * a) {
	// Here we add all lists of files to project
	// and we add all flags to project.

	// std::map<std::string, std::vector<fs::path>> filteredMap;
	//
	// std::sort(fileNames.begin(), fileNames.end(), [](const fs::path & a, const fs::path & b) {
	// 		return a.string() < b.string();
	// 	});

	for (auto & f : a->filteredMap["sources"]) {
	   cout << f << endl;
		// addSrc(e, addon.filesToFolders.at(e));
	}

	// ADD Include
	for (auto & f : a->filteredMap["includes"]) {
		for (auto & c : buildConfigs) {
			addCommand("Add :objects:" + c + ":buildSettings:HEADER_SEARCH_PATHS: string " + ofPathToString(f));
		}
	}

	// addLibrary function
	for (auto & f : a->filteredMap["libs"]) {
		for (auto & c : buildConfigs) {
			// FIXME: needed relative?
			addCommand("Add :objects:" + c + ":buildSettings:OTHER_LDFLAGS: string " + ofPathToString(fs::relative(f)));
		}
	}

	// addLDFLAG
	// 	for (auto & c : buildConfigs) {
	// addCommand("Add :objects:"+c+":buildSettings:OTHER_LDFLAGS: string " + ldflag);
	// }
	//
	// addCFLAG
	// 	for (auto & c : buildConfigs) {
	// FIXME: add array here if it doesnt exist
	// addCommand("Add :objects:"+c+":buildSettings:OTHER_CFLAGS: string " + cflag);
	// }
	// addDefine
	// 	for (auto & c : buildConfigs) {
	// FIXME: add array here if it doesnt exist
	// 	addCommand("Add :objects:"+c+":buildSettings:GCC_PREPROCESSOR_DEFINITIONS: string " + define);
	// }
	//
	// void xcodeProject::addCPPFLAG(const string& cppflag, LibType libType){
	// for (auto & c : buildConfigs) {
	// 	// FIXME: add array here if it doesnt exist
	// 	addCommand("Add :objects:"+c+":buildSettings:OTHER_CPLUSPLUSFLAGS: string " + cppflag);
	// }

}

std::string generateUUID(const string & input) {
	return uuidxx::uuid::Generate().ToString(false);
}

void copyTemplateFile::info() {
	alert("	copyTemplateFile info -------------", 92);
	alert("	from " + from.string(), 93);
	alert("	to " + to.string(), 93);
	for (auto & f : findReplaces) {
		std::cout << "	└─ Replacing " << f.first << " : " << f.second << std::endl;
	}
	std::cout << std::endl;
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
				std::cout << "	└─ Replacing " << f.first << " : " << f.second << std::endl;
			}

			for (auto & a : appends) {
				alert("APPEND ::: " + a, 96);
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

	for (auto & a : conf.addons) {
		alert("parsing addon " + a->name, 97);
		for (auto & f : a->filteredMap) {
			alert(">>>" + f.first);
			for (auto & s : f.second) {
				alert("   " + s.string(), 92);
			}
		}

		for (auto & f : a->filteredMap["includes"]) {
			std::string inc { "-I" + fs::path(a->path / f).string() };
			copyTemplateFiles[0].appends.emplace_back(inc);
		}
	}
}

void ofTemplateMacos::load() {
	alert("ofTemplateMacos::load()", 92);

	// ALL ABOUT FILES HERE
	auto projectName = fs::current_path().filename().string();
	// auto projectName = conf.projectPath.filename().string();
	fs::path xcodeProject { conf.projectPath / (projectName + ".xcodeproj") };
	cout << xcodeProject << endl;

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
	auto schemeFolder = conf.projectPath / (projectName + ".xcodeproj") / "xcshareddata/xcschemes";

	if (fs::exists(schemeFolder)) {
		fs::remove_all(schemeFolder);
	}
	fs::create_directories(schemeFolder);
	if (target == "osx" || target == "macos") {
		for (auto & f : { "Release", "Debug" }) {
			copyTemplateFiles.push_back({ path / ("emptyExample.xcodeproj/xcshareddata/xcschemes/emptyExample " + string(f) + ".xcscheme"),
				schemeFolder / (projectName + " " + f + ".xcscheme"),
				{ { "emptyExample", projectName } } });
		}

		copyTemplateFiles.push_back({ conf.projectPath / (projectName + ".xcodeproj/project.xcworkspace"),
			path / "emptyExample.xcodeproj/project.xcworkspace" });
	} else {

		// MARK:- IOS sector;
		copyTemplateFiles.push_back({ path / "emptyExample.xcodeproj/xcshareddata/xcschemes/emptyExample.xcscheme",
			schemeFolder / (projectName + ".xcscheme"),
			{ { "emptyExample", projectName } } });
	}

	// END ALL ABOUT FILES
	//
	addCommand("Add :_OFProjectGeneratorVersion string " + getPGVersion());

	// RENAME projectName inside templates
	// FIXME: review BUILT_PRODUCTS_DIR
	//

	// macos section
	std::map<std::string, std::string> uuid {
		{ "buildConfigurationList", "E4B69B5A0A3A1756003C02F2" },
		{ "buildActionMask", "E4B69B580A3A1756003C02F2" },
		// { "projRoot", "E4B69B4A0A3A1720003C02F2" },
		{ "frameworks", "E7E077E715D3B6510020DFD4" },
		{ "afterPhase", "928F60851B6710B200E2D791" },
		{ "buildPhases", "E4C2427710CC5ABF004149E2" },
		{ "", "" },
	};

	folderUUID = {
		{ "src", "E4B69E1C0A3A1BDC003C02F2" },
		{ "addons", "BB4B014C10F69532006C3DED" },
		{ "openFrameworks", "191EF70929D778A400F35F26" },
		{ "", "E4B69B4A0A3A1720003C02F2" }
		// { "localAddons",	"6948EE371B920CB800B5AC1A" },
	};

	if (target == "ios") {
		folderUUID = {
			{ "src", "E4D8936A11527B74007E1F53" },
			{ "addons", "BB16F26B0F2B646B00518274" },
			{ "", "29B97314FDCFA39411CA2CEA" },
			{ "Frameworks", "901808C02053638E004A7774" }
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

		buildConfigurations[0] = "1D6058940D05DD3E006BFB54"; // iOS Debug
		buildConfigurations[1] = "1D6058950D05DD3E006BFB54"; // iOS Release
		buildConfigurations[2] = "C01FCF4F08A954540054247B"; // iOS Debug
		buildConfigurations[3] = "C01FCF5008A954540054247B"; // iOS Release

		buildConfigs[0] = "1D6058940D05DD3E006BFB54"; // iOS Debug
		buildConfigs[1] = "1D6058950D05DD3E006BFB54"; // iOS Release
	}

	addCommand("Set :objects:" + uuid["buildConfigurationList"] + ":name " + projectName);

	// Just OSX here, debug app naming.
	if (target == "osx" || target == "macos") {
		// TODO: Hardcode to variable
		// FIXME: Debug needed in name?
		addCommand("Set :objects:E4B69B5B0A3A1756003C02F2:path " + projectName + "Debug.app");
	}

	// add sources

	// for (auto & f :)
	// add headers files
}

string ofTemplateMacos::getFolderUUID(const fs::path & folder, fs::path base) {
	//    alert ("xcodeProject::getFolderUUID " + folder.string() + " base:" + base.string());//+" : isfolder="+ofToString(isFolder)+" : base="+ base.string());
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

				//                alert("xcodeProject::getFolderUUID fullpath: " + fullPath.string(),33);

				// Query if partial path is already stored. if not execute this following block
				if (folderUUID.find(fullPath) != folderUUID.end()) {
					lastFolderUUID = folderUUID[fullPath];
					lastFolder = folderFromUUID[lastFolderUUID].string();
				}

				else {

					string thisUUID = generateUUID(fullPath);
					folderUUID[fullPath] = thisUUID;
					folderFromUUID[thisUUID] = fullPath;

					addCommand("");
					string folderName = ofPathToString(folders[a]);
					addCommand("Add :objects:" + thisUUID + ":name string " + folderName);

					addCommand("Add :objects:" + thisUUID + ":isa string PBXGroup");

					bool bFolderPathSet = false;

					if (folderName == "external_sources" || folderName == "local_addons") {

						//						addCommand("Add :objects:"+thisUUID+":sourceTree string SOURCE_ROOT");
						//						addCommand("Add :objects:"+thisUUID+":path string ");
						addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");

						bFolderPathSet = true;
					} else {
						if (lastFolderUUID == folderUUID[""]) { //} ||
							//                            lastFolder == "external_sources" || lastFolder == "local_addons") { //

							// Base folders can be in a different depth,
							// so we cut folders to point to the right path
							// ROY: THIS hardly makes sense to me. I can see the purpose of it. base2 is never set to anything.
							fs::path base2 { "" };
							size_t diff = folders.size() - (a + 1);
							for (size_t x = 0; x < diff; x++) {
								base2 = base2.parent_path();
							}
							//                            alert ("external_sources base = " + ofPathToString(base2) + " UUID: " + thisUUID, 33);
							addCommand("Add :objects:" + thisUUID + ":sourceTree string SOURCE_ROOT");
							addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(base2));
							bFolderPathSet = true;
						} else if (lastFolder == "external_sources" || lastFolder == "local_addons") { //
							addCommand("Add :objects:" + thisUUID + ":sourceTree string SOURCE_ROOT");
							//                            alert ("xxxx " + lastFolder + "  " + ofPathToString(base) + " UUID: " + thisUUID, 33);
							addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(getPathTo(base, folderName)));
							bFolderPathSet = true;
						} else {
							addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");
							//							fs::path addonFolder { fs::path(fullPath).filename() };
							addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(fullPath.filename()));
							bFolderPathSet = true;
						}
					}

					addCommand("Add :objects:" + thisUUID + ":children array");

					if (!bFolderPathSet) {
						if (folder.begin()->string() == "addons" || folder.begin()->string() == "src") { //} || folder.begin()->string() == "local_addons") {
							addCommand("Add :objects:" + thisUUID + ":sourceTree string <group>");
							//						fs::path addonFolder { fs::path(fullPath).filename() };
							addCommand("Add :objects:" + thisUUID + ":path string " + ofPathToString(fullPath.filename()));
							// alert ("group " + folder.string() + " : " + base.string() + " : " + addonFolder.string(), 32);
						} else {
							addCommand("Add :objects:" + thisUUID + ":sourceTree string SOURCE_ROOT");
						}
					}

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
