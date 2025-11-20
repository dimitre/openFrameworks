// #include "templates.h"
#include "ofTemplateVSCode.h"
#include "addons.h"


void ofTemplateVSCode::save() {
	alert("ofTemplateVSCode::save()", 92);
	workspace.data["generator"] = getVersion();
	workspace.save();
	cppProperties.save();
}



void ofTemplateVSCode::load() {
	alert("ofTemplateVSCode::load()", 92);
	// bool ok = ofTemplateMake::load();
	copyTemplateFiles.push_back({ path / ".vscode",
		conf.projectPath / ".vscode" });
	copyTemplateFiles.back().isFolder = true;

	workspace.fileName = conf.projectPath / (conf.projectName + ".code-workspace");
	cppProperties.fileName = conf.projectPath / ".vscode/c_cpp_properties.json";

	copyTemplateFiles.push_back({ path / "emptyExample.code-workspace",
		conf.projectPath / workspace.fileName });

	workspace.load();
	cppProperties.load();

	for (auto & a : conf.addons) {
		// alert("parsing addon " + a->name, 97);
		// for (auto & f : a->filteredMap) {
		// 	for (auto & s : f.second) {
		// 		alert("   " + s.string(), 92);
		// 	}
		// }

		// FIXME: This seems like an error. unused f
		for (auto & f : a->filteredMap["includes"]) {
			// workspace.addPath(a->path);
			workspace.addPath(f);
		}
	}
}
