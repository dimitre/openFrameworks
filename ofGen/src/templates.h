#pragma once

#include <fstream>
#include <iostream>

using std::cout;
using std::endl;
// using std::string;
// using std::vector;
#include "utils.h"
struct ofAddon;

std::string generateUUID(const string & input);


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

	virtual void addAddon(ofAddon * a) {
	    std::cout << "ofTemplate::addAddon() called on primitive member" << std::endl;
	};

	void info() {
		alert("ofTemplate::info " + name + ", path=" + path.string());
		for (auto & c : copyTemplateFiles) {
			c.info();
		}
	}

	virtual void load() {
		std::cout << "ofTemplate::load() called on primitive member" << std::endl;
	}

	// FIXME: uma funcao pra zerar o commands list quando sai de um projeto entra no proximo.

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
    void addAddon(ofAddon * a) override;

	ofTemplateMacos() {
		name = "macos";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	// void build() override {};


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

	std::string buildConfigurations[4] = {
		"E4B69B600A3A1757003C02F2", //macOS Debug
		"E4B69B610A3A1757003C02F2", //macOS Release

		"E4B69B4E0A3A1720003C02F2", //macOS Debug SDKROOT macosx
		"E4B69B4F0A3A1720003C02F2", //macOS Release SDKROOT macosx
	};

	std::string buildConfigs[2] = {
		"E4B69B610A3A1757003C02F2", //Release
		"E4B69B600A3A1757003C02F2", //Debug
	};

	std::map<fs::path, string> folderUUID;
	// Temporary
	std::map<string, fs::path> folderFromUUID;


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
