#include "ofTemplateVisualStudio.h"
#include "addons.h"

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
