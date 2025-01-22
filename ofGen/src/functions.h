
#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
// namespace fs = std::filesystem;
namespace fs = std::__fs::filesystem;

#include <vector>
#include <fstream> // ifstream
#include <iostream> // cout


#include <fmt/format.h>
#include <fmt/ranges.h>
#include <yaml-cpp/yaml.h>

using std::cout;
using std::endl;
using std::string;
using std::vector;


std::string getPlatformString() {
#ifdef __linux__
	string arch = execute_popen("uname -m");
	if (
		arch == "armv6l" ||
		arch == "armv7l" ||
		arch == "aarch64"
		) {
			return "linux" + arch;
		}
	else {
		return "linux64";
	}
#elif defined(__WIN32__)
	#if defined(__MINGW32__) || defined(__MINGW64__)
		return "msys2";
	#else
		return "vs";
	#endif
#elif defined(__APPLE_CC__)
//	return "osx";
	return "macos";
#else
	return {};
#endif
}


string colorText(const string & s, int color) {
	string c = std::to_string(color);
	return "\033[1;"+c+"m" + s + "\033[0m";
}

void alert(string msg, int color=33) {
	std::cout << colorText(msg, color) << std::endl;
}

// DIAM FONT
std::string sign = colorText( R"(
 ▗▄▖ ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖
▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌
▐▌ ▐▌▐▛▀▀▘▐▌▝▜▌▐▛▀▀▘▐▌ ▝▜▌
▝▚▄▞▘▐▌   ▝▚▄▞▘▐▙▄▄▖▐▌  ▐▌
                Prototype 0.01⚡️
)", 91)

+ colorText( R"(                Report issues on
                https://github.com/dimitre/ofLibs/
)", 92)
+
R"(
Now it is only possible to create projects inside
OF installation, three folders up. ex: of/apps/myApps/transcendence
to create a project there, first create the folder,
cd to the folder and invoke ofGen

)"
;


std::vector<std::string> fileToStrings (const fs::path & file) {
	vector<std::string> out;
	if (fs::exists(file)) {
		std::ifstream thisFile(file);
		string line;
		while(getline(thisFile, line)){
			out.emplace_back(line);
		}
	}
	return out;
}

#include <regex>
std::string stringReplace(const string & strIn, const string & from, const string & to) {
    return std::regex_replace(strIn, std::regex(from), to);
}

void ofStringReplace(string & strIn, const string & from, const string & to) {
    strIn = std::regex_replace(strIn, std::regex(from), to);
}

// trim from start (in place)
inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

std::string ofTrim(string line) {
    rtrim(line);
    ltrim(line);
    // line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());
    return line;
}



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

const vector<string> parseStates {
	"common",
	"linux",
	"linux64",
	"msys2",
	"macos",
	"ios",
};

string currentParseState { "" };

struct genConfig {
    fs::path ofPath = "../../..";
    string platform = getPlatformString();
} conf;


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





vector<string> splitStringOnceByLeft(const string &source, const string &delimiter) {
	size_t pos = source.find(delimiter);
	vector<string> res;
	if(pos == string::npos) {
		res.emplace_back(source);
		return res;
	}

	res.emplace_back(source.substr(0, pos));
	res.emplace_back(source.substr(pos + delimiter.length()));
	return res;
}

// parseConfig
//
//

std::map<string, vector<string> > addonProperties;




void parseConfig() {
    // fs::path fileName { "/Volumes/tool/ofw/addons/ofxOpenCv/addon_config.mk" };
    fs::path fileName { "../addons/ofxOpenCv/addon_config.mk" };

    // get addon libs, it can be none, one or multiple
    for (auto const & d : fs::directory_iterator { fileName.parent_path() / "libs" })  {
        if (fs::is_directory(d.path())) {
            alert("Lib inside: " + d.path().string(), 33);


            fs::path staticLibsFolder { d.path() / "lib" / "macos" };
            for (auto const & d : fs::directory_iterator { staticLibsFolder })  {
                alert(d.path().string(), 34);
            }

            fs::path includesFolder { d.path() / "include" };
            fs::recursive_directory_iterator it { includesFolder };
           	fs::recursive_directory_iterator last {  };

           	for(; it != last; ++it) {
          		if (fs::is_directory(it->path())) {
                    alert (it->path().string(), 35);
         			// folderListMap[path].emplace_back(it->path());
          		}
           	}
        }
    }



    // check all src folders
    //


    alert ("zed " + fileName.string(), 95);
    if (!fs::exists(fileName)) {
        return;
    }

    int lineNum=0;
   	for (auto & originalLine : fileToStrings(fileName)) {
        lineNum++;
        string line = originalLine;
        // not sure if it will work. I'm replacing with spaces. I need to remove them
        // std::replace( line.begin(), line.end(), '\r', ' ');
        // std::replace( line.begin(), line.end(), '\n', ' ');
        // ofStringReplace(line,"\n","");

        line = ofTrim(line);

      		// discard comments and blank lines
		if(line[0]=='#' || line == ""){
			continue;
		}
				// alert (line, 31);
        ofStringReplace(line," \\= ","=");
        ofStringReplace(line,"\\= ","=");
        ofStringReplace(line," \\=","=");
        ofStringReplace(line," \\+\\= ","+=");
        ofStringReplace(line," \\+\\=","+=");
        ofStringReplace(line,"\\+\\= ","+=");
				// ofStringReplace(line,"ADDON_LIBS","SEXOANAL");
				// alert (line, 32);

		// Trim., removing whitespace
        // line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());

		if(line[line.size()-1]==':'){
			ofStringReplace(line,":","");
			currentParseState = line;
		}

		if (
		  currentParseState != "common" &&
            currentParseState != "macos" &&
            currentParseState != "osx" &&
            currentParseState != "emscripten"
		) {
		continue;
		}

		// if (std::find(parseStates.begin(), parseStates.end(), currentParseState) == parseStates.end()) {
		// 	// ofLogError() << "Error parsing " << name << " addon_config.mk" << "\n\t\t"
		// 					// << "line " << lineNum << ": " << originalLine << "\n\t\t"
		// 					// << "sectionName " << currentParseState << " not recognized";
		// }
		// continue;

		if(line.find("=")!=string::npos){
			bool addToValue = false;
			string variable, value;
			vector<string> varValue;
			bool limpa = false;
			if(line.find("+=")!=string::npos){
				addToValue = true;
				varValue = splitStringOnceByLeft(line,"+=");
			}else{
			    limpa = true;
				addToValue = false;
				varValue = splitStringOnceByLeft(line,"=");
			}

			variable = ofTrim(varValue[0]);
			value = ofTrim(varValue[1]);
			if (limpa) {
			 addonProperties[variable].clear();
			}
			if (value != "") {
			addonProperties[variable].emplace_back(value);
			}


			// FIXME: This seems to be meaningless
			if(!checkCorrectPlatform(currentParseState)){
				continue;
			}

			if(!checkCorrectVariable(variable, currentParseState)){
				// ofLogError() << "Error parsing " << name << " addon_config.mk" << "\n\t\t"
								// << "line " << lineNum << ": " << originalLine << "\n\t\t"
								// << "variable " << variable << " not recognized for section " << currentParseState;
				continue;
			}


			alert ("currentParseState " + currentParseState, 94);
			alert ("line: " + line, 95);
			alert ("" + varValue[0], 93);
			alert ("" + varValue[1], 93);

			// parseVariableValue(variable, value, addToValue, originalLine, lineNum);
			cout << variable << endl;
			cout << value << endl;
			cout << addToValue << endl;
			cout << originalLine << endl;
			cout << lineNum << endl;

			cout << "------" << endl;
		}
    }

    //std::map<string, vector<string> > addonProperties;

    for (auto & a : addonProperties) {
        alert (a.first, 94);
        for (auto & p : a.second) {
            alert ("    " + p, 95);
        }
    }

    alert ("parseConfig end");

}
