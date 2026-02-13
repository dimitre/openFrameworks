#pragma once

#include <string>
#include <map>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;

struct ofTemplate;
struct ofAddon;

// ofProject is a determinate OF project, it can have multiple addons, and multiple templates.

struct ofAddon {
public:
	std::string currentParseState { "" };
	std::string name { "" };
	fs::path path;

	// All files scanned
	std::map<std::string, std::vector<fs::path>> filesMap;

	// All exclusion filter paths to be tested
	std::map<std::string, std::vector<fs::path>> exclusionsMap;

	// Remaining files after filtering.
	std::map<std::string, std::vector<fs::path>> filteredMap;

	std::map<std::string, std::vector<std::string>> addonProperties;

	bool isProject = false;
	bool isLocal = false;

	void load();
	void loadFiles();
	void relative();
	void loadAddonConfig();
	void refine();
	void showFiles();

	void scanFolder(const fs::path & path,
		std::map<std::string, std::vector<fs::path>> & filesMap,
		// std::map<std::string, std::vector<fs::path>> & exclusionsMap,
		bool recursive = false);
};


void parseConfigAllAddons();
void createTemplates();
