#pragma once

#include "utils.h"
struct ofAddon;
#include <map>

std::string generateUUID(const std::string & input);

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

	std::map<std::string, std::string> commands;

	std::vector<copyTemplateFile> copyTemplateFiles;

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
		}
	}

	void info() {
		// alert("	ofTemplate::info projectName=" + name + ", path=" + path.string(), 34);
	}

	void cleanTemplateFiles() {
		for (auto & c : copyTemplateFiles) {
			alert("will remove " + c.to.string(), 96);
		}
	}
};
