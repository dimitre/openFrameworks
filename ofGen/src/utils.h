


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


std::vector<std::string> textToVector (const fs::path & file) {
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
