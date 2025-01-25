#pragma once


#include "utils.h"

#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
namespace fs = std::__fs::filesystem;


#include <string>
#include <vector>
#include <map>

inline std::string getPlatformString() {
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




struct copyTemplateFile {
public:
	fs::path from;
	fs::path to;
	std::vector<std::pair<std::string, std::string>> findReplaces;
	std::vector<std::string> appends;
	bool run();
};


struct ofAddon {
public:
    std::string currentParseState { "" };
    std::string name { "" };
    fs::path path;
    std::map<std::string, std::vector<std::string> > addonProperties;
    void load();
};


struct ofTemplate {
public:
	std::string name { "" };
	fs::path path;
	ofTemplate() { }
	std::vector<copyTemplateFile> copyTemplateFiles;
	virtual void load() { }
	virtual void build() {
		for (auto & c : copyTemplateFiles) {
			c.run();
		}
	}

	// load to memory
};



#include <map>
static struct genConfig {
	fs::path ofPath { "../" };
	// it will be cwd unless project path is passed by variable.
	fs::path projectPath { "../apps/werkApps/Pulsar" };
	// std::string platform { getPlatformString() };
	// void setOFPath -  to set both ofPath and templatesPath ?
	//
	// void getFoldersRecursively(const fs::path & path, std::string platform);
	std::map<std::string, std::vector<fs::path>> filesMap;

	std::vector<std::string> projects { "zed", "vscode" };
	std::vector<std::string> platforms { getPlatformString(), "osx" };

	void showFiles() {
	   alert(":::::..... showFiles", 93);
		for (auto & f : filesMap) {
			alert(f.first + ":", 31);
			for (auto & s : f.second) {
				std::cout << s << std::endl;
			}
		}
	}

	void scanFolder(const fs::path & path, bool recursive = false);

	std::vector<ofAddon> ofAddons;
	std::vector<ofTemplate *> ofTemplates;
	// void scanFolderRecursive(const fs::path & path);
} conf;





struct ofTemplateMacos : public ofTemplate {
public:
	ofTemplateMacos() {
		name = "macos";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override {};
	void build() override {};
};

struct ofTemplateZed : public ofTemplate {
public:
	ofTemplateZed() {
		name = "zed";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override {};
	void build() override {};
};



struct ofProject {
public:
	vector<ofAddon *> addonsPointer;
	vector<ofTemplate *> templates;

	void build() {
		std::cout << "addonsPointer.size " << addonsPointer.size() << std::endl;
		std::cout << "templates.size " << templates.size() << std::endl;
	}
};


// void parseAddon( const fs::path & addonPath ) {
//
void gatherProjectInfo();
void parseConfigAllAddons();
void createTemplates();
