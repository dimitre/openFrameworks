
#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
// namespace fs = std::filesystem;
namespace fs = std::__fs::filesystem;

#include <fstream> // ifstream
#include <iostream> // cout
#include <regex>
#include <vector>

#include "utils.h"

std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
	return std::regex_replace(strIn, std::regex(from), to);
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

// void genConfig::getFoldersRecursively(const fs::path & path, std::string platform){
// Object to save files list. it can be global to the project and cleared in each new project. it can be per addon (and project), or per project only.

void genConfig::scanFolder(const fs::path & path) {
	// it should exist and be a folder.
	if (!fs::exists(path)) return;
	if (!fs::is_directory(path)) return;

	// TODO: This can be converted to recursive_directory, but we have to review if the function isFolderNotCurrentPlatform works correctly in this case.

	fs::recursive_directory_iterator it { path };
	fs::recursive_directory_iterator last {};

	for (; it != last; ++it) {
		auto f = it->path();

		// avoid hidden folders like .git etc.
		if (f.filename().c_str()[0] == '.') {
			it.disable_recursion_pending();
			continue;
		}

		auto ext = f.extension().string();

		if (fs::is_directory(f)) {
			if (ext == ".xcodeproj") {
				it.disable_recursion_pending();
				continue;
			}

			// on osx, framework is a directory, let's not parse it....
			if (ext == ".framework" || ext == ".xcframework") {
				// ADD To Frameworks List
				allFiles.frameworks.emplace_back(f);
				it.disable_recursion_pending();
				continue;
			} else {
				allFiles.includes.emplace_back(f);
				// ADD To includes list
			}

		} else {
			// bool platformFound = false;

			// if (!platform.empty() && f.string().find(platform) != std::string::npos) {
			// 	platformFound = true;
			// }

			if (ext == ".a" || ext == ".lib" || ext == ".dylib" || ext == ".so" || (ext == ".dll" && platform != "vs")) {
				allFiles.libs.emplace_back(f);
				// if (platformFound) {

				// LibraryBinary lib(f);
				// if(lib.isValidFor(arch, target)){
				// libLibs.push_back(lib);
				// }
				// }
				// FIXME: todo REGEX /\.(pdf|jpg)$/. // Split in headers and sources?

			} else if (ext == ".h" || ext == ".hpp" || ext == ".c" || ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".m" || ext == ".mm") {
				allFiles.sources.emplace_back(f);
				// libFiles.emplace_back(f);
			}
		}
	}

	// 	// TODO: disable recursion pending... it is not recursive yet.
	// 	if ((path.extension() != ".framework") || (path.extension() != ".xcframework")) {
	// 		for (const auto & entry : fs::directory_iterator(path)) {
	// 			auto f = entry.path();
	// 			if (f.filename().c_str()[0] == '.') continue; // avoid hidden files .DS_Store .vscode .git etc
	//             bool shouldCheckPlatform = true;
	//             if (fs::is_directory(f) && countSubdirectories(f) > 2 && f.string().find("src") != std::string::npos) {
	//                 shouldCheckPlatform = false;
	// //                cout << "getFoldersRecursively shouldCheckPlatform = false : " << f.filename().string() << endl;
	//             }

	//             if (fs::is_directory(f) && (!shouldCheckPlatform || !isFolderNotCurrentPlatform(f.filename().string(), platform))) {
	//                 getFoldersRecursively(f, folderNames, platform);
	//             }
	// 		}
	// 		folderNames.emplace_back(path.string());
	// 	}
}
