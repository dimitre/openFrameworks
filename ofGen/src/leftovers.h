

bool checkCorrectVariable(const string & variable, const string & state){
	// if (state == "meta") {
	// 	return std::find(AddonMetaVariables.begin(),
	// 					 AddonMetaVariables.end(),
	// 					 variable) != AddonMetaVariables.end();
	// }
	if ( state == "macos" || state == "common" ) {
		return std::find(AddonProjectVariables.begin(),
						 AddonProjectVariables.end(),
						 variable) != AddonProjectVariables.end();
	} else {
		return checkCorrectPlatform(state);
	}
}




bool checkCorrectPlatform(const string & state) {
	if (state == "meta" || state == "common") {
		return true;
	}
	if (std::find(parseStates.begin(), parseStates.end(), state) != parseStates.end()) {
		if (conf.platform == state) {
			return true;
		}
	}
	return false;
}


const vector<string> parseStates {
	"common",
	"linux",
	"linux64",
	"msys2",
	"macos",
	"ios",
};



// const vector<string> AddonMetaVariables {
// 	"ADDON_NAME",
// 	"ADDON_DESCRIPTION",
// 	"ADDON_AUTHOR",
// 	"ADDON_TAGS",
// 	"ADDON_URL",
// };

const vector<string> AddonProjectVariables = {
	"ADDON_DEPENDENCIES",

	"ADDON_INCLUDES",
	"ADDON_CFLAGS",
	"ADDON_CPPFLAGS",
	"ADDON_LDFLAGS",
	"ADDON_LIBS",
	"ADDON_DEFINES",

	"ADDON_SOURCES",
	"ADDON_HEADER_SOURCES",
	"ADDON_C_SOURCES",
	"ADDON_CPP_SOURCES",
	"ADDON_OBJC_SOURCES",

	"ADDON_LIBS_EXCLUDE",
	"ADDON_LIBS_DIR",
	"ADDON_SOURCES_EXCLUDE",
	"ADDON_INCLUDES_EXCLUDE",
	"ADDON_FRAMEWORKS_EXCLUDE",

	"ADDON_DATA",

	"ADDON_PKG_CONFIG_LIBRARIES",
	"ADDON_FRAMEWORKS",
	"ADDON_DLLS_TO_COPY",
	"ADDON_ADDITIONAL_LIBS",
};




// FIXME: This seems to be meaningless
			if(!checkCorrectPlatform(currentParseState)){
				continue;
			}

			if(!checkCorrectVariable(variable, currentParseState)){
				continue;
			}
