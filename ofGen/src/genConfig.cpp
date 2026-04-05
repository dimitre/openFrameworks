#include "genConfig.h"
genConfig conf;
#include "ofTemplate.h"

#include "ofTemplateChalet.h"
#include "ofTemplateMacos.h"
#include "ofTemplateVSCode.h"
#include "ofTemplateZed.h"
#include <fstream>
#include <algorithm>

class genConfig::Impl {
public:
    YAML::Node ofYaml;
    // Any other YAML-related helpers
    std::vector<std::string> nodeToStrings(const YAML::Node& yaml, const std::string& index);
};

genConfig::genConfig() : pImpl(std::make_unique<Impl>()) {}
genConfig::~genConfig() = default;

bool genConfig::isTemplateAddedToProject(std::string val) {
	return std::find(templateNames.begin(), templateNames.end(), val) != templateNames.end();
}

void genConfig::listAddonsAsYaml() {
	alert ("listAddonsAsYaml", 96);
	auto addonsFolder { ofPath / "addons" };
	if (fs::exists(addonsFolder)) {
		alert ("folder yes", 96);

		YAML::Node addonsList(YAML::NodeType::Sequence);
		for (auto const & d : fs::directory_iterator { addonsFolder }) {
			if (fs::is_directory(d.path())) {
				addonsList.push_back(d.path().filename().string());
			}
		}
		std::cout << addonsList << std::endl;
	} else {
		alert ("no folder", 96);

	}
}


void genConfig::import() {
	if (fs::exists("of.yml")) {
		alert("of.yml already present", 32);
	} else {
		if (!isValidProjectLocation()) {
			alert("⚠️  not a valid ofWorks project folder. exiting", 91);
			alert(fs::current_path().string(), 91);
			// alert("⚠️  Not an ofWorks project folder, no action taken", 94);

		} else {
			std::ifstream file("addons.make");
			YAML::Node node;
			node["ofpath"] = "../../..";
			node["version"] = "0.0.1";
			node["templates"] = std::vector { "chalet", "zed" };

			if (file.is_open()) {
				std::string line;
				while (std::getline(file, line)) {
					// msg(line, 33);
					// next line trims whitespace at the end.
					line = line.substr(0, line.find_last_not_of(" \f\n\r\t\v") + 1);
					node["addons"].push_back(line);
				}
			}
			file.close();

			std::ofstream ofYamlOut("of.yml");
			// cout << node << endl;
			ofYamlOut << node;
			ofYamlOut << "\n\n"; // blank lines
			ofYamlOut << "# addons:\n";
			ofYamlOut << "#   - ofxOpencv\n";
			ofYamlOut << "#   - ofxMath\n";

			ofYamlOut.close();
			cout << endl;
			alert("ok, of.yml created from addons.make", 32);
		}
	}
}

bool genConfig::loadYML() {
	fs::path configFile { "of.yml" };
	if (!fs::exists(configFile)) {
		// alert("no of.yml present. use `ofgen import` to create one from addons.make", 32);
		return false;
		// alert("missing of.yml file ", 31);
	} else {
		// config = YAML::LoadFile(configFile);
		//
		// TODO: Idea, I can keep ofYaml inside conf, so more info can be parsed only if needed, like infoPlist in Chalet/macos template.
		pImpl->ofYaml = YAML::LoadFile(configFile.string());
		if (pImpl->ofYaml["ofpath"]) { // use ofpath only if the key exists.
			auto ofPathYML = pImpl->ofYaml["ofpath"];
			ofPath = ofPathYML.as<std::string>();
		}

		//"icon",
		for (const auto & s : std::vector<std::string> { "version" }) {
			if (pImpl->ofYaml[s]) {
				conf.settings[s] = pImpl->ofYaml[s].as<std::string>();
			}
		}

		if (pImpl->ofYaml["infoPlist"]) { //info.plist
			// alert("--- infoPlist entry", 96);
			YAML::Node infoNode = pImpl->ofYaml["infoPlist"];
			for (YAML::const_iterator it = infoNode.begin(); it != infoNode.end(); ++it) {
				std::string key { it->first.as<std::string>() };
				std::string value { it->second.as<std::string>() };
				// alert("--- infoPlist " + key + " = " + value, 96);
				// Handle bool (YES/NO) vs string values
				conf.infoPlist[key] = value;
			}
		}

		if (pImpl->ofYaml["agents"]) {
			YAML::Node agentsNode = pImpl->ofYaml["agents"];
			if (agentsNode.IsScalar()) {
				agentsContent = agentsNode.as<std::string>();
			} else if (agentsNode.IsSequence()) {
				for (std::size_t i = 0; i < agentsNode.size(); i++) {
					agentsContent += agentsNode[i].as<std::string>() + "\n\n";
				}
			}
		}

		conf.addonsNames = nodeToStrings("addons");

		if (pImpl->ofYaml["addonsSources"]) {
			for (auto c : pImpl->ofYaml["addonsSources"]) {
				auto name { c["name"].as<std::string>() };
				auto repo { c["repository"].as<std::string>() };
				if (fs::exists(ofPath / "addons" / name)) {
					// FIXME: Check if it exists
					// Transform to a unique set...
					//
					if (std::find(conf.addonsNames.begin(), conf.addonsNames.end(), name) == conf.addonsNames.end()) {
						conf.addonsNames.emplace_back(name);
					} else {
						cout << "addon already added" << endl;
					}
					cout << name << endl;
					cout << repo << endl;
					cout << "------" << endl;
				}
			}
		}

		if (empty(conf.templateNames)) {
			auto templateNames = nodeToStrings("templates");
			if (templateNames.size() > 0) {
				conf.templateNames = templateNames;
			} else {
				// FIXME: this is an error in linux64 (no template with this name)
				// better idea is having a lookup table with platforms and default templates.
				// and apply directly to ofgen import
			}
		} else {
			alert("using templates from command line parameter");
		}
		cout << endl;

		if (!conf.templateNames.size()) {
			alert("No templates selected, exiting", 95);
			exit(0);
		}

		// FIXME: no lugar disso fazer mesmo um map<string, vector<string>> pra parsear todos de uma vez.
		conf.frameworks = nodeToStrings("frameworks");
		conf.defines = nodeToStrings("defines");

		// FIXME: Maybe rename to additional Sources?
		conf.additionalSources = nodeToPaths("sources");
		if (conf.additionalSources.size()) {
			alert("Additional Source Directories ");
			for (auto & a : conf.additionalSources) {
				cout << a << endl;
			}
		}

		if (conf.addonsNames.size()) {
			alert("Addons");
			// for (auto & a : conf.addonsNames) {
			// 	cout << a << endl;
			// }
			cout << joinStrings(conf.addonsNames, ", ") << endl;
		}

		if (conf.platforms.size()) {
			alert("Platforms");
			cout << joinStrings(conf.platforms, ", ") << endl;
		} else {
			alert("No Platforms Yet");
		}
		cout << endl;
	}

	return true;
}

std::vector<std::string> genConfig::nodeToStrings(const std::string & index) {
	std::vector<std::string> out;
	if (pImpl->ofYaml[index]) {
		auto items = pImpl->ofYaml[index];
		for (std::size_t i = 0; i < items.size(); i++) {
			out.emplace_back(items[i].as<std::string>());
		}
	}
	return out;
}

std::vector<fs::path> genConfig::nodeToPaths(const std::string & index) {
	std::vector<fs::path> out;
	if (pImpl->ofYaml[index]) {
		auto items = pImpl->ofYaml[index];
		for (std::size_t i = 0; i < items.size(); i++) {
			out.emplace_back(items[i].as<std::string>());
		}
	}
	return out;
}

void genConfig::open() {
	for (auto & t : templates) {
		if (t->commands.count("open")) {
			int result = std::system(t->commands["open"].c_str());
			if (result != 0) {
				std::cerr << "Warning: Failed to execute command: " << t->commands["open"]
						  << " (exit code: " << result << ")" << std::endl;
			}
		}
	}
}

int genConfig::build() {
	std::cout << std::endl;
	alert("BUILDING", 5);
	for (auto & t : templates) {
		if (t->commands.count("build")) {
			return std::system(t->commands["build"].c_str());
		}
	}
	return -1;
}

void genConfig::run() {
	for (auto & t : templates) {
		if (t->commands.count("run")) {
			int result = std::system(t->commands["run"].c_str());
			if (result == 0) {
				std::cout << "Success." << std::endl;
			} else {
				std::cerr << "Fail with error " << result << std::endl;
			}
		}
	}
}

bool genConfig::dist() {
	alert("distProject", 95);
	if (pImpl->ofYaml["distFolder"]) {
		std::string folder { pImpl->ofYaml["distFolder"].as<std::string>() };
		std::string command { "cp -rP bin/*.app " + folder };
		alert(command, 95);
		return std::system(command.c_str());
	}
	return false;
}

bool genConfig::bundleProject() {
	alert("bundleProject", 5);
	for (auto & t : templates) {
		// alert(t->name, 5);
		if (t->commands.count("bundle")) {
			alert(t->commands["bundle"], 95);
			return std::system(t->commands["bundle"].c_str());
		}
	}
	return false;
}

bool genConfig::bump() {
	fs::path configFile { "of.yml" };

	// Load existing YAML or create new node
	YAML::Node yaml;
	if (fs::exists(configFile)) {
		yaml = YAML::LoadFile(configFile.string());
	}

	// Get current version or default to 0.0.0
	std::string version = "0.0.0";
	if (yaml["version"]) {
		version = yaml["version"].as<std::string>();
	}

	// Parse version (format: major.minor.patch)
	std::vector<std::string> parts = ofSplitString(version, ".");
	while (parts.size() < 3) {
		parts.emplace_back("0");
	}

	// Increment patch version (minor bump)
	try {
		int patch = std::stoi(parts[2]);
		patch++;
		parts[2] = std::to_string(patch);
	} catch (...) {
		parts[2] = "1";
	}

	// Reconstruct version string
	std::string newVersion = parts[0] + "." + parts[1] + "." + parts[2];
	yaml["version"] = newVersion;

	// Write back to file
	std::ofstream outFile(configFile);
	if (!outFile.is_open()) {
		std::cerr << "Failed to open " << configFile << " for writing" << std::endl;
		return false;
	}

	outFile << yaml;
	outFile.close();

	alert("Bumped version: " + version + " -> " + newVersion, 92);

	// Update settings so buildProject sees the new version
	settings["version"] = newVersion;

	return true;
}

bool genConfig::buildProject() {

	// ofProject project;
	//
	if (!isValidProjectLocation()) {

		// MV desc to inside isValidProjectLocation check
		alert("⚠️  not a valid ofWorks project folder, no action taken", 91);
		alert(fs::current_path().string(), 91);
		alert("no src folder found, no of.yml file or addons.make found and no OF installed in default path ../../..");
		alert("use ```ofgen help``` to see more options");

		// ◢██
		// build = false;
		return false;
		// conf.help();
	}

	bool hasYml = loadYML();
	if (hasYml) {
		alert("of.yml found, building from there", 95);
	} else {

		if (!isValidOfPath()) {
			alert("⚠️  OF not found in default path " + ofPath.string());
			help();
			return false;
		} else {
			alert("of path OK, proceeding");

			if (!fs::exists("bin")) {
				alert("bin folder not found, creating");
				fs::create_directory("bin");
			}
		}

		alert("building from addons.make", 95);
		fs::path addonsListFile { projectPath / "addons.make" };
		if (fs::exists(addonsListFile)) {
			addonsNames = textToVector(addonsListFile);
		} else {
			alert("no addons.make found", 95);
		}

		alert("No templates found (of.yml or parameters), ofgen will deduce from platform", 95);

		std::map<std::string, std::vector<std::string>> platformTemplates {
			{ "macos", { "chalet", "zed", "macos" } },
			{ "vs", { "chalet", "zed" } },
			{ "linux64", { "chalet", "zed" } },
			{ "linuxaarch64", { "chalet", "zed" } },
			// { "vs", { "visualstudio" } },
			// { "msys2", { "make", "vscode" } },
			// { "linux64", { "make", "vscode" } },
		};

		std::string platform { getPlatformString() };
		cout << "platform is " << platform << endl;
		if (!empty(platform)) {
			if (empty(templateNames)) {
				templateNames = platformTemplates[platform];
			} else {
			}
		}

		alert("Templates ");
		cout << joinStrings(templateNames, ", ") << endl;
	}

	// scanFolder()
	// create templates, add to project
	for (auto & t : templateNames) {
		if (t == "chalet") {
			templates.emplace_back(new ofTemplateChalet());
			project.templates.emplace_back(templates.back());
		} else if (t == "zed") {
			templates.emplace_back(new ofTemplateZed());
			project.templates.emplace_back(templates.back());
		} else if (t == "macos") {
			templates.emplace_back(new ofTemplateMacos());
			project.templates.emplace_back(templates.back());
		} else if (t == "vscode") {
			templates.emplace_back(new ofTemplateVSCode());
			project.templates.emplace_back(templates.back());
		}
		// else if (t == "make") {
		// 	templates.emplace_back(new ofTemplateMake());
		// 	project.templates.emplace_back(templates.back());
		// }

		// else if (t == "visualstudio") {
		// 	templates.emplace_back(new ofTemplateVisualStudio());
		// 	project.templates.emplace_back(templates.back());
		// }

		else {
			alert("invalid template name : " + t + ", exiting", 95);
			return false;
			// std::exit(1);
		}


	}

	// load templates, show info of each template
	// for (auto & t : templates) {
	// 	// cout << t->name << " : " << t->path << endl;
	// 	t->load();
	// 	t->info();
	// }

	// now parse project addons, or yml

	if (!fs::exists("./src")) {
		// FIXME: check if template is ios and copy mm files accordingly. if not copy src files from templates.
		fs::path from { ofPath / "scripts" / "templates" / "src" };
		fs::path to { "./src" };
		alert(from.string(), 95);
		alert(fs::current_path().string(), 95);
		try {
			fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::update_existing);
		} catch (fs::filesystem_error & e) {
			std::cerr << "error copying template file " << from << " : " << to << std::endl;
			return false;
		}
	}
	// exit(1);

	// DELICATE. treating projects as an addon.
	// it works well. not delicate anymore.
	// src will always exist because we copy them if not.
	// if (fs::exists("./src"))


	// THIS Adds the project as it was an addon. Beautiful
	addons.push_back(new ofAddon());
	ofAddon * addon = addons.back();
	addon->isProject = true;
	addon->isLocal = true;
	addon->name = "ProjectSourceFiles_" + projectName;
	addon->path = "";

	for (auto & f : frameworks) {
		addon->filesMap["frameworks"].emplace_back(f);
	}

	// addon->showFiles();
	// addon->info();
	for (auto & path : additionalSources) {
		addon->filesMap["includes"].emplace_back(path);
	}
	addon->load();
	// addons.emplace_back(addon);
	project.addons.emplace_back(addons.back());


	// TODO: Add here additional sources
	// for (auto & a : additionalSources) {
	// 	alert(">> Additional Sources Folder: " + a.string(), 95);
	// 	addons.push_back(new ofAddon());
	// 	ofAddon * addon = addons.back();
	// 	addon->isProject = true;
	// 	addon->name = "AdditionalSource_" + projectName;
	// 	addon->path = a;
	// 	addon->isProject = true;

	// 	scanFolder(a, addon->filesMap, true);
	// 	addon->load();
	// 	project.addons.emplace_back(addons.back());
	// }

	// else {
	// 	alert("NO SRC FILE FOUND IN PROJECT", 95);
	// 	std::exit(1);
	// }

	// fs::path addonsListFile { projectPath / "addons.make" };
	// if (fs::exists(addonsListFile)) {
	// vector<std::string> addonsList { textToVector(addonsListFile) };
	// vector<std::string> addonsList = { "ofxMidi" }; //ofxMidi ofxOpenCv

	for (auto & l : addonsNames) {
		if (l != "") {
			addons.push_back(new ofAddon());
			ofAddon * addon = addons.back();

			// ofAddon addon;
			addon->name = l;
			// check if local addon exists, if not check in of addons folder.
			if (fs::exists(projectPath / l)) {
				addon->isLocal = true;
				addon->path = projectPath / l;
			} else {
				if (fs::exists(ofPath / "addons" / l)) {
					addon->path = ofPath / "addons" / l;
				}
			}

			if (std::empty(addon->path)) {
				continue;
			}

			addon->load();
			// addons.emplace_back(addon);
			project.addons.emplace_back(addons.back());
		}
	}
	// }

	// pass files to projects.
	project.build();

	if (!agentsContent.empty()) {
		std::ofstream agentsFile("AGENTS.md");
		if (agentsFile.is_open()) {
			agentsFile << R"(# Project Context

This is an **ofWorks** project (an openFrameworks fork).

## Math Types

Do **not** use legacy openFrameworks math types. Use their GLM equivalents instead:

| Don't use | Use instead |
|-----------|-------------|
| `ofPoint` | `glm::vec3` |
| `ofVec2f` | `glm::vec2` |
| `ofVec3f` | `glm::vec3` |
| `ofVec4f` | `glm::vec4` |
| `ofMatrix3x3` | `glm::mat3` |
| `ofMatrix4x4` | `glm::mat4` |
| `ofQuaternion` | `glm::quat` |
| `ofVectorMath` | GLM equivalents |

## Math Constants

Use GLM constants instead of legacy ones:

| Don't use | Use instead |
|-----------|-------------|
| `PI` | `glm::pi<float>()` |
| `M_PI` | `glm::pi<float>()` |
| `TWO_PI` | `glm::two_pi<float>()` |
| `HALF_PI` | `glm::half_pi<float>()` |

## Build Workflow

Project files (Chalet, Xcode) are **auto-generated** by `ofgen` from `of.yml`. Do **not** manually edit `chalet.yaml` or `.xcodeproj` files.

```sh
ofgen             # regenerate project files
chalet buildrun   # build and run
```

## Project-specific notes

)";
			agentsFile << agentsContent << "\n";
			agentsFile.close();
			alert("Generated AGENTS.md", 92);
		}
	}

	return true;
}
