#pragma once

#include "utils.h"

#include <map>
#include <string>
#include <vector>

static inline std::string getPGVersion() {
	return "ofGen v0.1";
}

static inline std::string ofPathToString(const fs::path & path) {
	try {
		return path.string();
	} catch (fs::filesystem_error & e) {
		std::cerr << "ofPathToString: error converting fs::path to string " << e.what();
	}
	return {};
}


struct ofAddon {
public:
	std::string currentParseState { "" };
	std::string name { "" };
	fs::path path;

	// Testing it here now.
	std::map<std::string, std::vector<fs::path>> filesMap;
	std::map<std::string, std::vector<fs::path>> exclusionsMap;
	std::map<std::string, std::vector<fs::path>> filteredMap;

	std::map<std::string, std::vector<std::string>> addonProperties;

	void load();
	void loadFiles();
	void relative();
	void loadAddonConfig();
	void refine();
	void showFiles();
};





void scanFolder(const fs::path & path,
	std::map<std::string, std::vector<fs::path>> & filesMap,
	// std::map<std::string, std::vector<fs::path>> & exclusionsMap,
	bool recursive = false);


/*
ofProject is a determinate OF project, it can have multiple addons, and multiple templates.
*/
struct ofProject {
public:
	fs::path path;
	vector<ofAddon *> addons;
	vector<ofTemplate *> templates;
	void build();

};

// void parseAddon( const fs::path & addonPath ) {
//
void gatherProjectInfo();
void parseConfigAllAddons();
void createTemplates();
void buildProject();



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
        5, 7, 30, 31, 32, 33, 34, 35, 36,
        90, 91, 92, 93, 94, 95, 96,
        // 41, 42, 43, 44, 45, 46, 47,
        // 100, 101, 102, 103, 104, 105, 106, 107,
    };

    for (auto & a : colors ){
        cout << colorText (std::to_string(a) + "███  " , a) ;
    }
    cout << endl;
}
