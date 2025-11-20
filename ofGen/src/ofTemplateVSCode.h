#include "templates.h"
#include <fstream>

struct ofTemplateVSCode : public ofTemplate {
public:
	ofTemplateVSCode() {
		name = "vscode";
		path = conf.ofPath / "scripts" / "templates" / name;

		openCommand = "vscode . ";
	}
	void load() override;
	void save() override;

	struct fileJson {
		fs::path fileName;
		json data;

		// only works for workspace
		void addPath(fs::path folder) {
			std::string path = folder.is_absolute() ? folder.string() : "${workspaceRoot}/../" + folder.string();
			json object;
			object["path"] = path;
			json::json_pointer p = json::json_pointer("/folders");
			data[p].emplace_back(object);
		}

		void addToArray(string pointer, fs::path value) {
			json::json_pointer p = json::json_pointer(pointer);
			if (!data[p].is_array()) {
				data[p] = json::array();
			}

			std::string path = fs::path(value).is_absolute() ? value.string() : "${workspaceRoot}/" + value.string();
			data[p].emplace_back(path);
		}

		void load() {
			if (!fs::exists(fileName)) {
				std::cerr << "JSON file not found " << fileName.string() << std::endl;
				return;
			}

			std::ifstream ifs(fileName);
			try {
				data = json::parse(ifs);
			} catch (json::parse_error & ex) {
				std::cerr << "JSON parse error at byte" << ex.byte << std::endl;
				std::cerr << "fileName" << fileName.string() << std::endl;
			}
		}

		void save() {
			//		alert ("saving now " + fileName.string(), 33);
			//		std::cout << data.dump(1, '\t') << std::endl;
			std::ofstream jsonFile(fileName);
			try {
				jsonFile << data.dump(1, '\t');
			} catch (std::exception & e) {
				std::cerr << "Error saving json to " << fileName.string() << ": " << e.what() << std::endl;
			}
		}
	};

	fileJson workspace;
	fileJson cppProperties;

	// void addAddon(ofAddon * a) override;
};
