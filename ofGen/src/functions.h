
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

#include "utils.h"


string currentParseState { "" };

static struct genConfig {
    // fs::path ofPath { "../../.." };
    fs::path ofPath { "../" };
    // fs::path projectPath { fs::current_path() };
    fs::path projectPath { "../apps/werkApps/Pulsar" };
    string platform { getPlatformString() };
    // void setOFPath -  to set both ofPath and templatesPath ?
} conf;


static void divider() {
    // cout << colorText(colorText("-----------------------------------------------------------", 5), 92) << endl;
     cout << colorText("-----------------------------------------------------------", 92) << endl;
}

static void replaceAll(std::string& str, const std::string& from, const std::string& to) {
	if(from.empty())
		return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

// maybe not needed. replace by a normal split string.
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



struct copyTemplateFile {
public:
	fs::path from;
	fs::path to;
	std::vector <std::pair <string, string>> findReplaces;
	std::vector <std::string> appends;
	bool run() {

	if (fs::exists(from)) {
		// ofLogVerbose() << "copyTemplateFile from: " << from << " to: " << to;
		alert("base::copyTemplateFile from: " + from.string() + " to: " + to.string(), 33);

		if (findReplaces.size() || appends.size()) {
			// Load file, replace contents, write to destination.
			std::ifstream fileFrom(from);
			std::string contents((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
			fileFrom.close();

			for (auto & f : findReplaces) {
				// Avoid processing empty pairs
				if (empty(f.first) && empty(f.second)) {
					continue;
				}
				replaceAll(contents, f.first, f.second);
				// ofLogVerbose() << "└─ Replacing " << f.first << " : " << f.second;
				cout << "└─ Replacing " << f.first << " : " << f.second << endl;
			}

			for (auto & a : appends) {
			     // alert(a, 35);
			    contents += "\n" + a;
			}

			std::ofstream fileTo(to);
			try{
				fileTo << contents;
			}catch(std::exception & e){
				std::cerr << "Error saving to " << to << endl;
				std::cerr << e.what() << endl;
				return false;
			}catch(...){
				std::cerr << "Error saving to " << to << endl;

				return false;
			}


		} else {
			// straight copy
			try {
				fs::copy(from, to, fs::copy_options::update_existing);
			}
			catch(fs::filesystem_error & e) {
				std::cerr << "error copying template file " << from << " : " << to << endl;
				std::cerr << e.what() << endl;
				return false;
			}
		}
	} else {
		return false;
	}
	return true;
	}
};


struct ofAddon {
public:
    string name;
    fs::path path;
    std::map<string, vector<string> > addonProperties;
};

struct ofTemplate {
    public:
    string name { "" };
    fs::path path;
    ofTemplate() { }
    vector <copyTemplateFile> copyTemplateFiles;
    virtual void load() {}
    virtual void build() {
        for (auto & c : copyTemplateFiles) {
            c.run();
        }
    }

    // load to memory
};

struct ofTemplateMacos : public ofTemplate {
    public:
    ofTemplateMacos() {
        name = "macos";
        path = conf.ofPath / "scripts" / "templates" / name;
    }
    void load() override {};
    void build() override {};
};

struct ofTemplateZed : public ofTemplate {
    public:
    ofTemplateZed() {
        name = "zed";
        path = conf.ofPath / "scripts" / "templates" / name;
    }
    void load() override {};
    void build() override {};
};

vector <ofAddon> ofAddons;
vector <ofTemplate*> ofTemplates;

struct ofProject {
public:
    vector <ofAddon*> addonsPointer;
    vector <ofTemplate*> templates;
};

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

void parseConfig() {
    // parseAddon("../addons/ofxOpenCv");
    alert ("parseConfig begin");
    for (auto const & d : fs::directory_iterator { "../addons" })  {
        if (fs::is_directory(d.path())) {
            parseAddon(d.path());
        }
    }
    alert ("parseConfig end");

}
