#include "ofTemplateChalet.h"
#include "addons.h"
#include <fstream>

// #include <chrono>
// #include <format>

// std::string timeString() {
//     using namespace std::chrono;

//     const auto now = system_clock::now();
//     const auto tt  = system_clock::to_time_t(now);

//     std::tm tm{};
//     localtime_r(&tt, &tm);           // local time

//     return std::format("{:04}{:02}{:02}-{:02}{:02}",
//                        tm.tm_year + 1900,
//                        tm.tm_mon  + 1,
//                        tm.tm_mday,
//                        tm.tm_hour,
//                        tm.tm_min);
// }
//
#include <chrono>
#include <fmt/chrono.h> // fmt’s chrono integration

std::string timeString() {
	using namespace std::chrono;
	// const auto now = system_clock::now();
	// const auto zt  = current_zone()->to_local(now);   // local time point
	// return fmt::format("{:%Y%m%d-%H%M}", zt);          // e.g. 202512031137

	return fmt::format("{:%Y%m%d-%H%M}", system_clock::now()); // GMT / UTC
}

void ofTemplateChalet::load() {
	alert("ofTemplateChalet::load()", 92);

	fs::path projectFrom { path / "chalet.yaml" };

	std::vector<std::string> addonsNames;
	for (auto & a : conf.addons) {
		addonsNames.emplace_back(a->name);
	}

	projectYaml = YAML::LoadFile(projectFrom.string());

	projectYaml["name"] = conf.projectName;
	projectYaml["variables"]["platform"] = getPlatformString();
	projectYaml["variables"]["addons"] = joinStrings(addonsNames, ",");
	projectYaml["variables"]["generator"] = getVersion();
	projectYaml["variables"]["time"] = timeString();

	if (fs::exists("icon") && fs::is_directory("icon")) {
		for (auto const & i : fs::directory_iterator { "icon" }) {
			if (i.path().extension() == ".ico") {
				alert("icon found " + i.path().string(), 95);
				projectYaml["targets"]["empty"]["settings:Cxx"]["windowsApplicationIcon"] = i.path().string();
			} else if (i.path().extension() == ".png") {
				alert("icon found " + i.path().string(), 95);
				projectYaml["distribution"]["empty"]["linuxDesktopEntry"]["icon"] = i.path().string();
				projectYaml["distribution"]["empty"]["linuxDesktopEntry"]["template"] = "platform/linux/app.desktop";
				projectYaml["distribution"]["empty"]["macosBundle"]["icon"] = i.path().string();
			}
		}
	}

	// MARK: Not ok to remove since addonToChalet only include defines from ADDON_DEFINES, not from of.yml define
	for (auto & d : conf.defines) {
		projectYaml["targets"]["empty"]["settings:Cxx"]["defines"].push_back(d);
	}

	// for (auto & f : conf.frameworks) {
	// 	projectYaml["abstracts:*"]["appleFrameworks"].push_back(f);
	// }

	// for (auto & a : conf.addons) {
	// for (auto & l : a->filteredMap["libs"]) {
	// 	string libPath { "${var:ofPath}/addons/" + a->name + '/' + l.string() };
	// 	projectYaml["targets"]["empty"]["settings:Cxx"]["staticLinks"].push_back(libPath);
	// }
	// for (auto & f : a->filteredMap["includes"]) {
	// }
	// }
}

void ofTemplateChalet::addAddon(ofAddon * a) {
	// alert(" ofTemplateChalet::addAddon() " + a->name, 91);

	for (auto & f : a->filteredMap["sources"]) {
		std::string folder = a->isProject ? "" : "${var:ofPath}/addons/" + a->name + "/";
		std::string path { folder + f.generic_string() };
		projectYaml["targets"]["empty"]["files"]["include"].push_back(path);
	}
	// for (auto & f : a->filteredMap["headers"]) {
	// }

	for (auto & f : a->filteredMap["includes"]) {
		std::string folder = a->isProject ? "" : "${var:ofPath}/addons/" + a->name + "/";
		std::string path { folder + f.string() };

		// projectYaml["abstracts:*"]["settings:Cxx"]["includeDirs"].push_back(path);
		projectYaml["targets"]["empty"]["settings:Cxx"]["includeDirs"].push_back(path);
	}

	for (auto & f : a->filteredMap["libs"]) {
		// fs::path p { a->path / f };
		// std::string path { "${var:ofPath}/addons/" + a->name + "/" + f.string() };

		std::string folder = a->isProject ? "" : "${var:ofPath}/addons/" + a->name + "/";
		std::string path { folder + f.string() };
		projectYaml["targets"]["empty"]["settings:Cxx"]["staticLinks"].push_back(path);
	}

	if (a->addonProperties.count("ADDON_FRAMEWORKS")) {
		for (const auto & f : a->addonProperties["ADDON_FRAMEWORKS"]) {
			for (const auto & s : ofSplitString(f, " ")) {
				// alert("     appleFramework " + s, 95);
				alert("	└─ appleFramework " + s, 94);
				// projectYaml["abstracts:*"]["settings:Cxx"]["appleFrameworks"].push_back(s);
				projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworks"].push_back(s);
			}
		}
	}

	// FIXME: TODO: handle cflags etc.
	const std::map<std::string, std::string> addonToChalet {
		{ "ADDON_CFLAGS", "compileOptions" },
		{ "ADDON_CPPFLAGS", "compileOptions" },
		{ "ADDON_LDFLAGS", "linkerOptions" },
		{ "ADDON_DEFINES", "defines" },
	};

	for (auto & param : addonToChalet) {
		if (a->addonProperties.count(param.first)) {
			for (const auto & property : a->addonProperties[param.first]) {
				for (const auto & s : ofSplitString(property, " ")) {
					alert("	└─ " + param.second + " : " + s, 94);
					projectYaml["targets"]["empty"]["settings:Cxx"][param.second].push_back(s);
				}
			}
		}
	}

	for (const fs::path & f : a->filteredMap["frameworks"]) {
		// addFramework(a->path / f);
		alert("	└─ appleFramework " + f.string(), 94);
		projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworks"].push_back(f.string());
	}

	// I'm now removing this one. it was handled already by conf.defines in general addon loading. ofAddon::loadFiles populating conf.defines
	// for (auto & p : a->addonProperties["ADDON_DEFINES"]) {
	// 	projectYaml["targets"]["empty"]["settings:Cxx"]["defines"].push_back(p);
	// }
}

void ofTemplateChalet::save() {

	renameYamlKey(projectYaml["targets"], "empty", conf.projectName);
	projectYaml["distribution"]["empty"]["buildTargets"] = conf.projectName;
	renameYamlKey(projectYaml["distribution"], "empty", conf.projectName);

	renameYamlKey(projectYaml["distribution"]["empty.dmg"]["positions"], "empty", conf.projectName);
	renameYamlKey(projectYaml["distribution"], "empty.dmg", conf.projectName + ".dmg");

	// Change key "empty" to project name in targets
	// {
	// 	auto targets = projectYaml["targets"];
	// 	YAML::Node emptyNode = targets["empty"];
	// 	targets[conf.projectName] = emptyNode;
	// 	targets.remove("empty");
	// }

	// // Change key "empty" to project name in distribution
	// {
	// 	auto distribution = projectYaml["distribution"];
	// 	YAML::Node emptyNode = distribution["empty"];
	// 	distribution[conf.projectName] = emptyNode;
	// 	distribution.remove("empty");
	// }

	alert("ofTemplateChalet::save()", 92);

	fs::path projectTo { conf.projectPath / "chalet.yaml" };
	// alert(projectTo.string(), 35);
	std::ofstream saveFile(projectTo.string());
	// std::string comment = "generator " + getVersion();
	// saveFile << YAML::Comment(comment) << projectYaml;
	saveFile << projectYaml;
	saveFile.close();
}
