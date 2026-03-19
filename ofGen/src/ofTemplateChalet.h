#pragma once

#include "ofTemplate.h"
struct genConfig;
extern genConfig conf;

#include <yaml-cpp/yaml.h>

struct ofTemplateChalet : public ofTemplate {
public:
	YAML::Node projectYaml;

	ofTemplateChalet();
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
