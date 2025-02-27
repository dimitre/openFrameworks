#pragma once

#include "utils.h"
// extern genConfig conf;

// #include <fstream>

// using std::cout;
// using std::endl;
// using std::string;
// using std::vector;
struct ofAddon;

std::string generateUUID(const string & input);

struct copyTemplateFile {
public:
	fs::path from;
	fs::path to;
	std::vector<std::pair<std::string, std::string>> findReplaces;
	std::vector<std::string> appends;
	bool isFolder = false;
	// void (ofTemplate::*transform)(std::string &) = nullptr;

	std::string contents;
	bool isLoaded = false;

	void load();
	// std::function<void(std::string &)> transform = nullptr;
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
		alert("	ofTemplate::info projectName=" + name + ", path=" + path.string(), 34);
		// for (auto & c : copyTemplateFiles) {
		// 	c.info();
		// }
	}

	virtual void addSrc(const fs::path & srcFile, const fs::path & folder) {
		std::cout << "	ofTemplate::addSrc() called on primitive member " << name << std::endl;
	};

	virtual void addAddon(ofAddon * a) {
		// std::cout << "ofTemplate::addAddon() called on primitive member " << name << std::endl;
	};

	virtual void load() {
		std::cout << "ofTemplate::load() called on primitive member " << name << std::endl;
	}

	virtual void save() {
		std::cout << "ofTemplate::save() called on primitive member " << name << std::endl;
	}

	virtual void edit(std::string & str) {
    	std::cout << "ofTemplate::edit() called on primitive member " << name << std::endl;
	};

	// FIXME: uma funcao pra zerar o commands list quando sai de um projeto entra no proximo.
	virtual void build() {
		alert("	ofTemplate::build " + name + ", path=" + path.string(), 34);
		for (auto & c : copyTemplateFiles) {
			c.run();
			// c.info();
		}
	}
};

// inline void transf(string & s) {
// 	s = "ARWIL";
// }

struct ofTemplateMacos : public ofTemplate {
public:
	ofTemplateMacos() {
		name = "macos";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	void save() override;
	void addAddon(ofAddon * a) override;
	void addFramework(const fs::path & path);
	void edit(std::string & str) override;

	// static inline void transOK(std::string & s) {
	// 	s = "ARWIL";
	// }

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

	// macos section
	std::map<std::string, std::string> uuid {
		{ "buildConfigurationList", "E4B69B5A0A3A1756003C02F2" },
		{ "buildActionMask", "E4B69B580A3A1756003C02F2" },
		// { "projRoot", "E4B69B4A0A3A1720003C02F2" },
		{ "frameworks", "E7E077E715D3B6510020DFD4" },
		{ "afterPhase", "928F60851B6710B200E2D791" },
		{ "buildPhases", "E4C2427710CC5ABF004149E2" },
		{ "", "" },
	};

	void addSrc(const fs::path & srcFile, const fs::path & folder) override;
	string addFile(const fs::path & path, const fs::path & folder, const fileProperties & fp);

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

	std::map<fs::path, std::string> extensionToFileType {
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

	// std::string buildConfigurations[4] = {
	// 	"E4B69B600A3A1757003C02F2", //macOS Debug
	// 	"E4B69B610A3A1757003C02F2", //macOS Release

	// 	"E4B69B4E0A3A1720003C02F2", //macOS Debug SDKROOT macosx
	// 	"E4B69B4F0A3A1720003C02F2", //macOS Release SDKROOT macosx
	// };

	// std::string buildConfigs[2] = {
	// 	"E4B69B610A3A1757003C02F2", //Release
	// 	"E4B69B600A3A1757003C02F2", //Debug
	// };

	std::string buildConfigs[4] = {
		"E4B69B600A3A1757003C02F2", //macOS Debug
		"E4B69B610A3A1757003C02F2", //macOS Release

		"E4B69B4E0A3A1720003C02F2", //macOS Debug SDKROOT macosx
		"E4B69B4F0A3A1720003C02F2", //macOS Release SDKROOT macosx
	};

	std::map<fs::path, string> folderUUID;
	// Temporary
	std::map<string, fs::path> folderFromUUID;
};

struct ofTemplateMake : public ofTemplate {
public:
	ofTemplateMake() {
		name = "make";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	void save() override;
};

struct ofTemplateZed : public ofTemplate {
public:
	ofTemplateZed() {
		name = "zed";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	void save() override;
};

struct ofTemplateVSCode : public ofTemplate {
public:
	ofTemplateVSCode() {
		name = "vscode";
		path = conf.ofPath / "scripts" / "templates" / name;
	}
	void load() override;
	void save() override;


	// void addAddon(ofAddon * a) override;
};
