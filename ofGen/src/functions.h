#pragma once

// #include <fmt/format.h>
// #include <fmt/ranges.h>
// #include <yaml-cpp/yaml.h>

#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
// namespace fs = std::filesystem;
namespace fs = std::__fs::filesystem;

#include <vector>
#include <iostream> // cout
#include <map>

using std::cout;
using std::endl;
using std::string;
using std::vector;


string currentParseState { "" };

#include "utils.h"

static struct genConfig {
    fs::path ofPath { "../" };
    // it will be cwd unless project path is passed by variable.
    fs::path projectPath { "../apps/werkApps/Pulsar" };
    string platform { getPlatformString() };
    // void setOFPath -  to set both ofPath and templatesPath ?
} conf;


#include "structs.h"


void createTemplates() {
    vector <string> templateNames { "zed", "macos" };
    for (const auto & t : templateNames) {
        if (t == "zed") {
            ofTemplates.emplace_back(new ofTemplateZed());
        }
        else if (t == "macos") {
            ofTemplates.emplace_back(new ofTemplateMacos());
        }
        // etc.
    }
    alert ("createTemplates", 92);
    for (auto & t : ofTemplates) {
        cout << t->name << endl;
        cout << t->path << endl;
    }
    cout << ofTemplates.size() << endl;
}




// void parseAddon( const fs::path & addonPath ) {
void parseAddon( const string & name ) {
    // moved to inside the function so it resets for each addon. desirable?
    //

    // Test if it is local
    fs::path addonPath { conf.projectPath / name };
    if (fs::exists(addonPath)) {
        // ok
    } else {
        addonPath = conf.ofPath / "addons" / name;
        if (!fs::exists(addonPath)) {
            cout << "no addon path found " << name << endl;
            return;
        }
    }
    cout << "addon found, addon path: " << addonPath << endl;

    std::map<string, vector<string> > addonProperties;

    // fs::path fileName { "/Volumes/tool/ofw/addons/ofxOpenCv/addon_config.mk" };
    fs::path fileName { addonPath / "addon_config.mk" };
    // fs::path fileName { addonPath / "../addons/ofxOpenCv/addon_config.mk" };

    // get addon libs, it can be none, one or multiple
    fs::path libsPath { fileName.parent_path() / "libs" };
    if (fs::exists(libsPath))
    for (auto const & d : fs::directory_iterator {  })  {
        if (fs::is_directory(d.path())) {
            alert("Lib inside: " + d.path().string(), 33);


            fs::path staticLibsFolder { d.path() / "lib" / "macos" };
            if (fs::exists(staticLibsFolder))

            for (auto const & d : fs::directory_iterator { staticLibsFolder })  {
                alert(d.path().string(), 34);
            }

            fs::path includesFolder { d.path() / "include" };
            if (fs::exists(includesFolder)) {

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
    }

    alert ("zed " + fileName.string(), 91);
    if (!fs::exists(fileName)) {
        return;
    }

    int lineNum=0;
   	for (auto & originalLine : textToVector(fileName)) {
        lineNum++;
        string line = originalLine;
        // not sure if it will work. I'm replacing with spaces. I need to remove them
        // std::replace( line.begin(), line.end(), '\r', ' ');
        // std::replace( line.begin(), line.end(), '\n', ' ');
        // stringReplace(line,"\n","");

        line = ofTrim(line);

      		// discard comments and blank lines
		if(line[0]=='#' || line == ""){
			continue;
		}
				// alert (line, 31);
        stringReplace(line," \\= ","=");
        stringReplace(line,"\\= ","=");
        stringReplace(line," \\=","=");
        stringReplace(line," \\+\\= ","+=");
        stringReplace(line," \\+\\=","+=");
        stringReplace(line,"\\+\\= ","+=");
				// stringReplace(line,"ADDON_LIBS","SEXOANAL");
				// alert (line, 32);

		// Trim., removing whitespace
        // line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());

		if(line[line.size()-1]==':'){
			stringReplace(line,":","");
			currentParseState = line;
		}

		if (
		  currentParseState != "common"
            && currentParseState != "macos"
            && currentParseState != "osx"
            // && currentParseState != "emscripten"
		) {
		continue;
		}

		// if (std::find(parseStates.begin(), parseStates.end(), currentParseState) == parseStates.end()) {
		// 	// std::cerr << "Error parsing " << name << " addon_config.mk" << "\n\t\t"
		// 					// << "line " << lineNum << ": " << originalLine << "\n\t\t"
		// 					// << "sectionName " << currentParseState << " not recognized";
		// }
		// continue;

		if(line.find("=")!=string::npos){
			bool addToValue = false;
			vector<string> varValue;
			bool limpa = false;
			if (line.find("+=")!=string::npos) {
				addToValue = true;
				// FIXME: maybe not needed. a simple split is ok.
				varValue = splitStringOnceByLeft(line,"+=");
			} else {
			    limpa = true;
				addToValue = false;
				varValue = splitStringOnceByLeft(line,"=");
			}

			// variable = ofTrim(varValue[0]);
			// value = ofTrim(varValue[1]);
			string variable = varValue[0];
			string value = varValue[1];
			if (limpa) {
			 addonProperties[variable].clear();
			}
			if (value != "") {
			addonProperties[variable].emplace_back(value);
			}


		}
    }

    for (auto & a : addonProperties) {
        alert ("    "+a.first, 94);
        for (auto & p : a.second) {
            alert ("       " + p, 95);
        }
    }
    //std::map<string, vector<string> > addonProperties;

}

void createMacosProject() {
    alert ("createMacosProject", 92);
    ofTemplates.emplace_back(new ofTemplateMacos());
    ofProject project;
    project.templates.emplace_back(ofTemplates.back());

    // now parse project addons, or yml
    fs::path addonsFile { conf.projectPath / "addons.make" };
    if (fs::exists(addonsFile)) {
        for (auto & l : textToVector(addonsFile)) {
            divider();

            cout << l << endl;
            parseAddon(l);
        }
    }

}

void parseConfigAllAddons() {
    alert ("parseConfig begin");
    for (auto const & d : fs::directory_iterator { conf.ofPath / "addons" })  {
        if (fs::is_directory(d.path())) {
            parseAddon(d.path());
        }
    }
    alert ("parseConfig end");
}
