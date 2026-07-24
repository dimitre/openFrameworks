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

	bool isTemplateAddedToProject(std::string val);

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
	std::string agentsContent;
	std::string bumpMessage; // set by bump(), reprinted at the end so it isn't buried


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

	void open();
	int build();
	bool bundleProject();
	bool dist();
	void run();
	// bump version in of.yml and build project again
	bool bump();

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
