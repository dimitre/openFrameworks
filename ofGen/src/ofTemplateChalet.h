#include "templates.h"

struct ofTemplateChalet : public ofTemplate {
public:
	YAML::Node projectYaml;

	ofTemplateChalet() {
		name = "chalet";
		path = conf.ofPath / "scripts" / "templates" / name;

		// openCommand = "chalet . ";
		buildCommand = "chalet build";
		runCommand = "chalet buildrun";
		cleanCommand = "chalet clean --all";
	}
	void load() override;
	void save() override;
	void addAddon(ofAddon * a) override;

	void renameYamlKey(YAML::Node node,
		const std::string & oldKey,
		const std::string & newKey) {
		if (node[oldKey]) {
			node[newKey] = std::move(node[oldKey]);
			node.remove(oldKey);
		}
	}
};
