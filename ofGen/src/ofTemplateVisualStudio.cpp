#include "ofTemplateVisualStudio.h"
#include "genConfig.h"
#include "addons.h"

ofTemplateVisualStudio::ofTemplateVisualStudio() {
	name = "visualstudio";
	path = conf.ofPath / "scripts" / "templates" / name;
	//FIXME - implement
	openCommand = "open " + conf.projectName + ".xcodeproj";
	// buildCommand = "msbuild";
	buildCommand = "powershell -c \"msbuild\"";
	// Untested
	cleanCommand = "powershell -c \"msbuild *.sln -t:Clean\"";
}


void ofTemplateVisualStudio::addAddon(ofAddon * a) {

#ifdef PORT
	ofLogVerbose("visualStudioProject::") << "Adding addon: [" << addon.name << "]";
	// Handle additional vcxproj files in the addon
	fs::path additionalFolder = addon.addonPath / (addon.name + "Lib");
	if (fs::exists(additionalFolder)) {
		for (const auto & entry : fs::directory_iterator(additionalFolder)) {
			auto f = entry.path();
			if (f.extension() == ".vcxproj") {
				additionalvcxproj.emplace_back(f);
			}
		}
	}
#endif

	for (auto & f : a->filteredMap["includes"]) {
		fs::path p = a->path / f;
		addInclude(p);
		// alert ("->" + p.string(), 95);
		// addCommand("Add :objects:" + c + ":buildSettings:HEADER_SEARCH_PATHS: string " + ofPathToString(p));
	}
}
