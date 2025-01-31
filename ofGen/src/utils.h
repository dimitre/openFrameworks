#pragma once

#include <iostream> // cout
#include <map>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

// static constexpr std::string_view VERSION = "ofGen v0.4";
using std::cout;
using std::endl;
using std::string;
using std::vector;

static inline std::string getPGVersion() {
	return "ofGen v0.4";
}

inline std::string colorText(const std::string & s, int color) {
	std::string c { std::to_string(color) };
	// return "\033[1;" + c + "m" + s + "\033[0m";
	return "\033[" + c + "m" + s + "\033[0m";
}

inline void alert(std::string msg, int color = 2) {
	std::cout << colorText(msg, color) << std::endl;
}

// DIAM FONT
const std::string sign = colorText(R"(
 ▗▄▖ ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖
▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌
▐▌ ▐▌▐▛▀▀▘▐▌▝▜▌▐▛▀▀▘▐▌ ▝▜▌
▝▚▄▞▘▐▌   ▝▚▄▞▘▐▙▄▄▖▐▌  ▐▌
                Prototype v0.4⚡️
)", 91)

	+ colorText(R"(                Report issues on
                https://github.com/dimitre/ofLibs/
)",
		92);

// )";

// Now it is only possible to create projects inside
// OF installation, three folders up. ex: of/apps/myApps/transcendence
// to create a project there, first create the folder,
// cd to the folder and invoke ofGen

bool ofIsPathInPath(const fs::path & path, const fs::path & base);
std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to);
std::vector<std::string> textToVector(const fs::path & file);
void replaceAll(std::string & str, const std::string & from, const std::string & to);

void ltrim(std::string & s);
void rtrim(std::string & s);
std::string ofTrim(std::string line);

std::vector<std::string> ofSplitString(const std::string & s, const std::string & delimiter);

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


static void divider() {
	// cout << colorText(colorText("-----------------------------------------------------------", 5), 92) << endl;
	std::cout << std::endl;
	std::cout << colorText("-----------------------------------------------------------", 90) << std::endl;
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

	// FIXME: implement platforms in addons from here.
	std::vector<std::string> platforms { getPlatformString(), "osx" };

	std::vector<std::string> addonsNames;

	std::vector<ofTemplate *> templates;

	std::vector<ofAddon *> addons;

	std::map<std::string, std::string> parametersMap;

	void updateFromParameters() {
		alert("genConfig::updateFromParameters()", 92);
		if (parametersMap.count("ofroot")) {
			ofPath = parametersMap["ofroot"];
		}
		if (parametersMap.count("templates")) {
			templateNames = ofSplitString(parametersMap["templates"], ",");
		}
		if (parametersMap.count("platforms")) {
			platforms = ofSplitString(parametersMap["platforms"], ",");
		}

		// TODO: ignore addons.make if addons are set via parameter
		// Write addons.make from this parameter if needed. or always
		if (parametersMap.count("addons")) {
			addonsNames = ofSplitString(parametersMap["addons"], ",");
		}
		if (parametersMap.count("path")) {
			fs::current_path(parametersMap["path"]);
		}
	}

	void help() {
		cout << R"(
ofGen without parameters will try to create a project on current folder.
and look for OF installation three folders up, like
cd of/apps/myApps/Transcend; ./ofGen
you can supply all project path, OF path, addons list, templates,
platforms as parameters like:
./ofGen templates=zed,macos,make platform=macos addons=ofxMidi,ofxOpencv ofpath=../../.. path=/Volumes/tool/Transcend
)" << endl;
	}

	// void scanFolderRecursive(const fs::path & path);
};

static genConfig conf;
