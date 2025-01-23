#pragma once

#include "addons.h"

// #include <fmt/format.h>
// #include <fmt/ranges.h>
// #include <yaml-cpp/yaml.h>

#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
// namespace fs = std::filesystem;
namespace fs = std::__fs::filesystem;

#include <iostream> // cout
#include <map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

#include "utils.h"

static struct genConfig {
	fs::path ofPath { "../" };
	// it will be cwd unless project path is passed by variable.
	fs::path projectPath { "../apps/werkApps/Pulsar" };
	string platform { getPlatformString() };
	// void setOFPath -  to set both ofPath and templatesPath ?
} conf;

#include "structs.h"

void createTemplates() {
	vector<string> templateNames { "zed", "macos" };
	for (const auto & t : templateNames) {
		if (t == "zed") {
			ofTemplates.emplace_back(new ofTemplateZed());
		} else if (t == "macos") {
			ofTemplates.emplace_back(new ofTemplateMacos());
		}
		// etc.
	}
	alert("createTemplates", 92);
	for (auto & t : ofTemplates) {
		cout << t->name << endl;
		cout << t->path << endl;
	}
	cout << ofTemplates.size() << endl;
}

// void parseAddon( const fs::path & addonPath ) {

void createMacosProject() {
	alert("createMacosProject", 92);
	ofTemplates.emplace_back(new ofTemplateMacos());
	ofProject project;
	project.templates.emplace_back(ofTemplates.back());

	// now parse project addons, or yml
	fs::path addonsListFile { conf.projectPath / "addons.make" };
	if (fs::exists(addonsListFile)) {
		for (auto & l : textToVector(addonsListFile)) {
			// divider();

			ofAddon addon;
			addon.name = l;
			if (fs::exists(conf.projectPath / l)) {
				addon.path = conf.projectPath / l;
			} else {
				if (fs::exists(conf.ofPath / "addons" / l)) {
					addon.path = conf.ofPath / "addons" / l;
				}
			}

			if (std::empty(addon.path)) {
				continue;
			}

			addon.load();
			// cout << l << endl;
			// parseAddon(l);

			ofAddons.emplace_back(addon);
		}
	}
}

void parseConfigAllAddons() {
	alert("parseConfig begin");
	for (auto const & d : fs::directory_iterator { conf.ofPath / "addons" }) {
		if (fs::is_directory(d.path())) {
			// parseAddon(d.path());
		}
	}
	alert("parseConfig end");
}
