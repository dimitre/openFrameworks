#pragma once

#include "uuidxx.h"

#include "utils.h"

#include <map>
#include <string>
#include <vector>

static inline std::string ofPathToString(const fs::path & path) {
	try {
		return path.string();
	} catch (fs::filesystem_error & e) {
		std::cerr << "ofPathToString: error converting fs::path to string " << e.what();
	}
	return {};
}

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

struct ofTemplate;

#include <map>
struct genConfig {
	fs::path ofPath { "../../.." };
	// it will be cwd unless project path is passed by variable.
	fs::path projectPath { "../apps/werkApps/Pulsar" };

	std::vector<std::string> projects { "zed", "vscode" };
	std::vector<std::string> platforms { getPlatformString(), "osx" };

	std::vector<ofAddon> ofAddons;
	std::vector<ofTemplate *> ofTemplates;
	// void scanFolderRecursive(const fs::path & path);
};

static genConfig conf;

struct copyTemplateFile {
public:
	fs::path from;
	fs::path to;
	std::vector<std::pair<std::string, std::string>> findReplaces;
	std::vector<std::string> appends;
	bool isFolder = false;
	bool run();
	void info();
};

struct ofTemplate {
public:
	std::string name { "" };
	fs::path path;
	ofTemplate() { }
	std::vector<copyTemplateFile> copyTemplateFiles;

	void info() {
		alert("ofTemplate::info " + name + ", path=" + path.string());
		for (auto & c : copyTemplateFiles) {
			c.info();
		}
	}
	virtual void load() {
		std::cout << "ofTemplate::load() called on primitive member" << std::endl;
	}
	virtual void build() {
		alert("ofTemplate::build " + name + ", path=" + path.string());
		for (auto & c : copyTemplateFiles) {
			c.run();
			// c.info();
		}
	}
};

struct ofTemplateMacos : public ofTemplate {
public:
	// FIXME: Provisory variable, to be handled by platform macos / ios in near future
	std::string target = "macos";

	std::string getFolderUUID(const fs::path & folder, fs::path base);

	std::vector<string> commands;
	bool debugCommands = false;

	void addCommand(const string & command) {
		if (debugCommands) {
			alert(command, 31);
		}
		commands.emplace_back(command);
	}

	fs::path getPathTo(fs::path path, string limit) {
		fs::path p;
		vector<fs::path> folders = std::vector(path.begin(), path.end());
		for (auto & f : folders) {
			p /= f;
			if (f.string() == limit) {
				//            alert("getPathTo "+  p.string(), 33);
				return p;
			}
		}
		return p;
	}

	const std::map<fs::path, string> extensionToFileType {
		{ ".framework", "wrapper.framework" },
		{ ".xcframework", "wrapper.xcframework" },
		{ ".dylib", "compiled.mach-o.dylib" },

		{ ".cpp", "sourcecode.cpp.cpp" },
		{ ".c", "sourcecode.cpp.c" },
		{ ".h", "sourcecode.cpp.h" },
		{ ".hpp", "sourcecode.cpp.h" },
		{ ".mm", "sourcecode.cpp.objcpp" },
		{ ".m", "sourcecode.cpp.objcpp" },

		{ ".xib", "file.xib" },
		{ ".metal", "file.metal" },
		{ ".xcconfig", "text.xcconfig" },

		{ ".entitlements", "text.plist.entitlements" },
		{ ".plist", "text.plist.xml" },
	};

	string buildConfigurations[4] = {
		"E4B69B600A3A1757003C02F2", //macOS Debug
		"E4B69B610A3A1757003C02F2", //macOS Release

		"E4B69B4E0A3A1720003C02F2", //macOS Debug SDKROOT macosx
		"E4B69B4F0A3A1720003C02F2", //macOS Release SDKROOT macosx
	};

	string buildConfigs[2] = {
		"E4B69B610A3A1757003C02F2", //Release
		"E4B69B600A3A1757003C02F2", //Debug
	};

	std::map<fs::path, string> folderUUID;
	// Temporary
	std::map<string, fs::path> folderFromUUID;

	string generateUUID(const string & input) {
		return uuidxx::uuid::Generate().ToString(false);
	}

	string generateUUID(const fs::path & path) {
		return generateUUID(path.string());
	}

	struct fileProperties {
		bool absolute = false;
		bool reference = true;
		bool addToBuildPhase = false;
		bool codeSignOnCopy = false;
		bool copyFilesBuildPhase = false;
		bool linkBinaryWithLibraries = false;
		bool addToBuildResource = false;
		bool addToResources = false;
		bool frameworksBuildPhase = false;
		bool isSrc = false;
		bool isGroupWithoutFolder = false;
		bool isRelativeToSDK = false;
	};

	ofTemplateMacos() {
		name = "macos";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	// void build() override {};
};

struct ofTemplateMake : public ofTemplate {
public:
	ofTemplateMake() {
		name = "make";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	// void build() override {};
};

struct ofTemplateZed : public ofTemplate {
public:
	ofTemplateZed() {
		name = "zed";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	// void build() override {};
};

bool ofIsPathInPath(const fs::path & path, const fs::path & base);

void scanFolder(const fs::path & path,
	std::map<std::string, std::vector<fs::path>> & filesMap,
	// std::map<std::string, std::vector<fs::path>> & exclusionsMap,
	bool recursive = false);

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
void infoTemplates();
void buildTemplates();
