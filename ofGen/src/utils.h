#pragma once

#include <iostream> // cout
#include <map>
#include <vector>

#include <filesystem>
namespace fs = std::filesystem;

#include <yaml-cpp/yaml.h>

// static constexpr std::string_view VERSION = "ofGen v0.4";
using std::cout;
using std::endl;
using std::string;
using std::vector;

static inline std::string getPGVersion() {
	return "ofGen v0.2.1";
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
Project Generator for OpenFrameworks (OFVISION)
                Prototype v0.2.1⚡️
)",
							 91)

	+ colorText(R"(                Report issues on
                https://github.com/dimitre/ofLibs/
)",
		92);

// )";

inline void testColors() {
	/*
    Color: 5 = blink white
    Color 7 : invert background
    30 : preto 31, 36 - cores
    41, 47 cores de fundo
    91/96 : cores vivas
    100/107 : cores vivas fundo

    */
	int colors[] = {
		// 5, //blinking
		0, 2, 5, 7, 30, 31, 32, 33, 34, 35, 36,
		90, 91, 92, 93, 94, 95, 96,
		// 41, 42, 43, 44, 45, 46, 47,
		// 100, 101, 102, 103, 104, 105, 106, 107,
	};

	cout << endl;
	for (auto & a : colors) {
		cout << colorText(std::to_string(a) + "███  ", a);
	}
	cout << endl;
	cout << endl;
}

// Now it is only possible to create projects inside
// OF installation, three folders up. ex: of/apps/myApps/transcendence
// to create a project there, first create the folder,
// cd to the folder and invoke ofGen

std::string textToString(const fs::path & file);
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

std::string ofPathToString(const fs::path & path);

// maybe not needed. replace by a normal split string.
// inline std::vector<std::string> splitStringOnceByLeft(const std::string & source, const std::string & delimiter) {
// 	size_t pos = source.find(delimiter);
// 	std::vector<std::string> res;
// 	if (pos == std::string::npos) {
// 		res.emplace_back(source);
// 		return res;
// 	}

// 	res.emplace_back(source.substr(0, pos));
// 	res.emplace_back(source.substr(pos + delimiter.length()));
// 	return res;
// }

static void divider() {
	// cout << colorText(colorText("-----------------------------------------------------------", 5), 92) << endl;
	std::cout << std::endl;
	std::cout << colorText("-----------------------------------------------------------", 90) << std::endl;
}

// extern genConfig conf;

struct ofTemplate;
struct ofAddon;

struct genConfig {

	std::string projectName { "" };
	fs::path ofPath { "../../.." };
	// it will be cwd unless project path is passed by variable.
	// fs::path projectPath { "../apps/werkApps/Pulsar" };
	fs::path projectPath { "." };

	// TESTING
	std::vector<fs::path> additionalSources;

	// vector <fs::path> projectPaths {
	// 	{ "../apps/werkApps/Pulsar" }
	// };
	// std::vector<std::string> templateNames { "macos", "zed", "make" }; //"vscode",
	std::vector<std::string> templateNames { "macos" }; //"vscode",
	// std::vector<std::string> templateNames; //"vscode",

	// FIXME: implement platforms in addons from here.
	std::vector<std::string> platforms { getPlatformString(), "osx" };

	std::vector<std::string> addonsNames;

	std::vector<std::string> frameworks;
	std::vector<std::string> defines;

	std::vector<ofTemplate *> templates;

	std::vector<ofAddon *> addons;

	std::map<std::string, std::string> parametersMap;
	// vector<std::string> singleParameters;
	std::string singleParameter;

	bool doesTemplateExist(std::string val) {
		return std::find(templateNames.begin(), templateNames.end(), val) != templateNames.end();
	}

	void parseParameters(const int argc, const char * argv[]) {
		// alert("parseParameters", 92);
		/*
addons : ofxMicroUI,ofxTools
ofroot : ../../..
path : .
templates : zed,macos
*/
		if (argc > 1) {
			for (int a = 1; a < argc; a++) {
				string param = argv[a];
				std::vector<std::string> parameters = ofSplitString(param, "=");
				if (parameters.size() == 2) {
					parametersMap[parameters[0]] = parameters[1];
				} else if (parameters.size() == 1) {
					// singleParameters.emplace_back(param);
					singleParameter = param;
					// cout << "parameters size " << parameters.size() << endl;
				}
			}

			if (parametersMap.count("ofpath")) {
				ofPath = parametersMap["ofpath"];
			}
			if (parametersMap.count("sources")) {
				// TODO: Additional sources here.
			}
			if (parametersMap.count("templates")) {
				alert("WOW templates clear", 95);
				cout << "inside conf, parseParameters() " << endl;
				cout << this << endl;

				templateNames.clear();
				templateNames = ofSplitString(parametersMap["templates"], ",");

				// for (auto & t : templateNames) {
				// 	alert(t, 95);
				// }
			}
			// if (parametersMap.count("platforms")) {
			// 	platforms = ofSplitString(parametersMap["platforms"], ",");
			// }

			// TODO: ignore addons.make if addons are set via parameter
			// Write addons.make from this parameter if needed. or always
			if (parametersMap.count("addons")) {
				addonsNames = ofSplitString(parametersMap["addons"], ",");
			}
			if (parametersMap.count("path")) {
				fs::current_path(parametersMap["path"]);
			}

			// alert("parametersMap ", 35);
			for (auto & p : parametersMap) {
				alert(p.first + " : " + p.second, 34);
			}
		} else {
			// Try to parse of.yml
			// if not, try to parse addons.make.
		}

		// Give projectName to general config
		projectName = fs::current_path().filename().string();

		if (doesTemplateExist("zed") || doesTemplateExist("vscode")) {
			if (!doesTemplateExist("make")) {
				templateNames.push_back("make");
			}
		}
	}

	bool isValidOfPath() {
		return fs::exists(ofPath / ".ofroot");
	}

	void help() {
		cout << R"(
ofGen without parameters will try to create a project on current folder.
and look for OF installation three folders up, like
cd of/apps/myApps/Transcend; ./ofGen
you can supply all project path, OF path, addons list, templates,
platforms as parameters like:
ofGen templates=zed,macos,make addons=ofxMidi,ofxOpencv ofpath=../../.. path=/Volumes/tool/Transcend
)" << endl;
	}

	void open() {
		std::string projectName = fs::current_path().filename().string();
		std::string command = "open " + projectName + ".xcodeproj";
		cout << command << endl;
		system(command.c_str());
	}

	void build() {
		// std::string command = "open " + projectName + ".xcodeproj";
		// cout << command << endl;
		system("xcodebuild");
	}

	YAML::Node config;
	void import();
	bool loadYML();
    vector<string> nodeToStrings(const string & index);
    vector<fs::path> nodeToPaths(const string & index);
	// void scanFolderRecursive(const fs::path & path);
};
// conf
// static genConfig conf;

extern genConfig conf;
