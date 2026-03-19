#include "ofTemplateZed.h"
#include "genConfig.h"
#include "addons.h"

ofTemplateZed::ofTemplateZed() {
	name = "zed";
	path = conf.getTemplatesFolder() / name;
	commands["open"] = "zed .";
}

void ofTemplateZed::load() {
	alert("ofTemplateZed::load()", 92);

	copyTemplateFiles.push_back({ path / "compile_flags.txt",
		conf.projectPath / "compile_flags.txt" });

	// copyTemplateFiles.push_back({ path / ".zed",
	// 	conf.projectPath / ".zed" });
	// copyTemplateFiles.back().isFolder = true;
	fs::path folder { conf.projectPath / ".zed" };
	if (!fs::exists(folder)) {
		fs::create_directory(folder);
	}

	copyTemplateFiles.push_back({ path / ".zed/keymap.json",
		conf.projectPath / ".zed/keymap.json" });
	copyTemplateFiles.push_back({ path / ".zed/settings.json",
		conf.projectPath / ".zed/settings.json" });
	copyTemplateFiles.push_back({ path / ".zed/tasks.json",
		conf.projectPath / ".zed/tasks.json" });

	// copyTemplateFiles.push_back({ path / "emptyExample.xcodeproj" / "project.pbxproj",
	// 	xcodeProject / "project.pbxproj",
	// 	{ { "emptyExample", conf.projectName },
	// 		rootReplacements } });

	copyTemplateFiles.push_back({ path / ".zed/debug.json",
		conf.projectPath / ".zed/debug.json",
		{ { "$PROJECT_NAME", conf.projectName } } });
}

void ofTemplateZed::save() {
	alert("ofTemplateZed::save()", 92);
	for (auto & a : conf.addons) {
		// alert("ofTemplateZed::save addon " + a->name, 95);
		// alert("ofTemplateZed::save filesMap includes " + a->name, 95);

		// for (auto & f : a->filesMap["includes"]) {
		//   cout << f << endl;
		// }
		// alert("ofTemplateZed::save filteredMap includes " + a->name, 95);

		// for (auto & f : a->filteredMap["includes"]) {
		//   cout << f << endl;
		// }
		for (auto & f : a->filteredMap["includes"]) {
			std::string inc { "-I" + fs::path(a->path / f).string() };
			copyTemplateFiles[0].appends.emplace_back(inc);
		}
	}
}
