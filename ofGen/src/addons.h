#pragma once

// #include "utils.h"
// #include <filesystem>
// namespace fs = std::filesystem;

#include <string>

#include <map>
#include <vector>

#include "utils.h"
// extern genConfig conf;

struct ofTemplate;

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

	bool isProject = false;

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
	std::vector<ofAddon *> addons;
	std::vector<ofTemplate *> templates;
	void build();
};

// void parseAddon( const fs::path & addonPath ) {
//
void gatherProjectInfo();
void parseConfigAllAddons();
void createTemplates();
void buildProject();
