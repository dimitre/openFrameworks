#pragma once

#include "utils.h"
#include "addons.h"
#include <vector>
#include <string>
// #include <yaml-cpp/yaml.h>

struct genConfig {

	genConfig();
	~genConfig();

	ofProject project;

	void listAddonsAsYaml();

	// // we can avoid including YAML everywhere by making shared_ptr / pimpl lang.
	// YAML::Node ofYaml;
 // Forward declare and hide YAML
    class Impl;
    std::unique_ptr<Impl> pImpl;

	fs::path getTemplatesFolder() {
		return ofPath / "scripts" / "templates";
	}
	// 16 jan 2026
	std::map<std::string, std::string> settings;
	// 19 jan 2026
	std::map<std::string, std::string> infoPlist;


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
	std::vector<std::string> templateNames; //"vscode",
	// std::vector<std::string> templateNames; //"vscode",

	// FIXME: implement platforms in addons from here.
	std::vector<std::string> platforms { getPlatformString() };

	std::vector<std::string> addonsNames;
	// std::set<std::string> addonsNames; //unordered_set

	std::vector<std::string> frameworks;
	std::vector<std::string> defines;

	std::vector<ofTemplate *> templates;

	std::vector<ofAddon *> addons;

	std::map<std::string, std::string> parametersMap;
	// vector<std::string> singleParameters;
	std::string singleParameter;

	std::string openCommand;
	std::string buildCommand;
	std::string runCommand;



	bool isTemplateAddedToProject(std::string val) {
		return std::find(templateNames.begin(), templateNames.end(), val) != templateNames.end();
	}

	void parseParameters(const int argc, const char * argv[]) {
		// alert ("platform string " + getPlatformString(), 95);
		// alert("parseParameters", 92);
		/*
addons : ofxMicroUI,ofxTools
ofroot : ../../..
path : .
templates : zed,macos
*/
		if (argc > 1) {
			for (int a = 1; a < argc; a++) {
				std::string param = argv[a];
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
				// alert("WOW templates clear", 95);
				// cout << "inside conf, parseParameters() " << endl;
				// cout << this << endl;

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

		if (isTemplateAddedToProject("zed") || isTemplateAddedToProject("vscode")) {
			if (!isTemplateAddedToProject("chalet")) {
				templateNames.push_back("chalet");
			}
		}
	}

	bool isValidOfPath() {
		return fs::exists(ofPath / ".ofroot");
	}

	bool isValidProjectLocation() {
		return !(
			!fs::exists("src") &&
			!fs::exists("of.yml") &&
			!fs::exists("addons.make") &&
			!fs::exists("../../../.ofroot")
		);
	}

	void help() {
		cout << R"(
help:
ofGen without parameters will try to create a project on current folder.
and look for OF installation three folders up, like
cd of/apps/myApps/Transcend; ./ofGen
you can supply all project path, OF path, addons list, templates,
platforms as parameters like:
ofGen templates=zed,macos,chalet addons=ofxMidi,ofxOpencv ofpath=../../.. path=/Volumes/tool/Transcend
)" << endl;
	}

	// void open();
	void open() {
		if (!empty(openCommand)) {
			int result = std::system(openCommand.c_str());
			if (result != 0) {
				std::cerr << "Warning: Failed to execute command: " << openCommand
						  << " (exit code: " << result << ")" << std::endl;
			}
		}
	}

	int build() {

		std::cout << std::endl;
		alert("BUILDING", 5);
		if (!empty(buildCommand)) {
			return std::system(buildCommand.c_str());
		}
		return -1;
		// std::string command = "open " + projectName + ".xcodeproj";
		// cout << command << endl;
		// system("xcodebuild");
	}

	void run() {
		if (!empty(runCommand)) {
			int result = std::system(runCommand.c_str());
			if (result == 0) {
				std::cout << "Success." << std::endl;
			} else {
				std::cerr << "Fail with error " << result << std::endl;
				// Further analysis of the return code might be needed based on the command
			}
		}
		// projectName = fs::current_path().filename().string();
		// std::string command = "open -n bin/" + projectName + ".app";
		// cout << command << endl;
		// system(command.c_str());
	}

	// void buildrun() {
	// 	std::string command { buildCommand };
	// 	if (!empty(runCommand)) {
	// 		command = " && " + runCommand;
	// 	}
	// 	std::system(command.c_str());
	// }

	void import();
	bool loadYML();
	std::vector<std::string> nodeToStrings(const std::string & index);
	std::vector<fs::path> nodeToPaths(const std::string & index);
	// void scanFolderRecursive(const fs::path & path);

	bool buildProject();
};
// conf
// static genConfig conf;

extern genConfig conf;
