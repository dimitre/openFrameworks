// void ofStringReplace(string & strIn, const string & from, const string & to) {
//     strIn = std::regex_replace(strIn, std::regex(from), to);
// }

#pragma once

#include <iostream> // cout
#include <map>
#include <vector>

// #if __has_include(<filesystem>)
// 	#include <filesystem>
// #else
// 	#include <experimental/filesystem>
// #endif
// // namespace fs = std::filesystem;
// namespace fs = std::__fs::filesystem;
//

#include <filesystem>
namespace fs = std::filesystem;

using std::cout;
using std::endl;
using std::string;
using std::vector;

bool ofIsPathInPath(const fs::path & path, const fs::path & base);
std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to);
std::vector<std::string> textToVector(const fs::path & file);
void replaceAll(std::string & str, const std::string & from, const std::string & to);

void ltrim(std::string & s);
void rtrim(std::string & s);
std::string ofTrim(std::string line);
// static std::string getPlatformString();


inline static std::string getPlatformString() {
#ifdef __linux__
	string arch = execute_popen("uname -m");
	if (
		arch == "armv6l" || arch == "armv7l" || arch == "aarch64") {
		return "linux" + arch;
	} else {
		return "linux64";
	}
#elif defined(__WIN32__)
	#if defined(__MINGW32__) || defined(__MINGW64__)
	return "msys2";
	#else
	return "vs";
	#endif
#elif defined(__APPLE_CC__)
	//	return "osx";
	return "macos";
#else
	return {};
#endif
}



// maybe not needed. replace by a normal split string.
inline std::vector<std::string> splitStringOnceByLeft(const std::string & source, const std::string & delimiter) {
	size_t pos = source.find(delimiter);
	std::vector<std::string> res;
	if (pos == std::string::npos) {
		res.emplace_back(source);
		return res;
	}

	res.emplace_back(source.substr(0, pos));
	res.emplace_back(source.substr(pos + delimiter.length()));
	return res;
}


inline std::string colorText(const std::string & s, int color) {
	std::string c { std::to_string(color) };
	return "\033[1;" + c + "m" + s + "\033[0m";
}

inline void alert(std::string msg, int color = 33) {
	std::cout << colorText(msg, color) << std::endl;
}

// DIAM FONT
const std::string sign = colorText(R"(
 ▗▄▖ ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖
▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌
▐▌ ▐▌▐▛▀▀▘▐▌▝▜▌▐▛▀▀▘▐▌ ▝▜▌
▝▚▄▞▘▐▌   ▝▚▄▞▘▐▙▄▄▖▐▌  ▐▌
                Prototype 0.01⚡️
)",
							 91)

	+ colorText(R"(                Report issues on
                https://github.com/dimitre/ofLibs/
)",
		92)
	+
	R"(
Now it is only possible to create projects inside
OF installation, three folders up. ex: of/apps/myApps/transcendence
to create a project there, first create the folder,
cd to the folder and invoke ofGen

)";

static void divider() {
	// cout << colorText(colorText("-----------------------------------------------------------", 5), 92) << endl;
	std::cout << std::endl;
	std::cout << colorText("-----------------------------------------------------------", 92) << std::endl;
}


struct ofTemplate;
struct ofAddon;

struct genConfig {
	fs::path ofPath { "../../.." };
	// it will be cwd unless project path is passed by variable.
	// fs::path projectPath { "../apps/werkApps/Pulsar" };
	fs::path projectPath { "." };
	// vector <fs::path> projectPaths {
	// 	{ "../apps/werkApps/Pulsar" }
	// };
	// std::vector<std::string> templateNames { "zed", "make", "macos" }; //"vscode",
	std::vector<std::string> templateNames { "macos" }; //"vscode",
	std::vector<std::string> platforms { getPlatformString(), "osx" };

	std::vector<ofTemplate *> templates;
	std::vector<ofAddon *> addons;
	// void scanFolderRecursive(const fs::path & path);
};

static genConfig conf;
