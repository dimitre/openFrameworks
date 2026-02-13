#pragma once

#include <iostream> // cout
static std::string version = "v0.9.7";

#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

#ifdef __linux__
	#include <sys/utsname.h>
#endif

// static constexpr std::string_view VERSION = "ofGen v0.4";
using std::cout;
using std::endl;

static inline std::string getVersion() {
	return "ofGen " + version;
}

inline std::string colorText(const std::string & s, int color) {
	std::string c { std::to_string(color) };
	// return "\033[1;" + c + "m" + s + "\033[0m";
	return "\033[" + c + "m" + s + "\033[0m";
}

inline void alert(std::string msg, int color = 2) {
	std::cout << colorText(msg, color) << std::endl;
}

// Now it is only possible to create projects inside
// OF installation, three folders up. ex: of/apps/myApps/transcendence
// to create a project there, first create the folder,
// cd to the folder and invoke ofGen

std::string textToString(const fs::path & file);
bool ofIsPathInPath(const fs::path & path, const fs::path & base);
std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to);
std::vector<std::string> textToVector(const fs::path & file);
void replaceAll(std::string & str, const std::string & from, const std::string & to);

void ltrim(std::string & s);
void rtrim(std::string & s);
std::string ofTrim(std::string line);

std::vector<std::string> ofSplitString(const std::string & s, const std::string & delimiter);

// static std::string getPlatformString();

inline static std::string getPlatformString() {
#ifdef __linux__
	// std::string arch = execute_popen("uname -m");
	// std::string arch = popen("uname -m");
	// #include <iostream>

	struct utsname sysinfo;
	uname(&sysinfo);
	std::string sysarch { "linux" + std::string(sysinfo.machine) };
	// FIXME: keept this way or change libs names?
	if (sysarch == "linuxx86_64") {
		sysarch = "linux64";
	}
	else if (sysarch == "linuxaarch64") {
	    sysarch = "rpi-aarch64";
	}
	// untested
	else if (sysarch == "linuxarmv6l") {
	    sysarch = "rpi-armv6l";
	}

	return sysarch;

	// if (
	// 	arch == "armv6l" || arch == "armv7l" || arch == "aarch64") {
	// 	return "linux" + arch;
	// } else {
	// 	return "linux64";
	// }
// #elif defined(__WIN32__)
#elif defined(_WIN32)
	// cout << "_WIN32" << endl;
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

std::string ofPathToString(const fs::path & path);

// maybe not needed. replace by a normal split string.
// inline std::vector<std::string> splitStringOnceByLeft(const std::string & source, const std::string & delimiter) {
// 	size_t pos = source.find(delimiter);
// 	std::vector<std::string> res;
// 	if (pos == std::string::npos) {
// 		res.emplace_back(source);
// 		return res;
// 	}

// 	res.emplace_back(source.substr(0, pos));
// 	res.emplace_back(source.substr(pos + delimiter.length()));
// 	return res;
// }

static inline void divider() {
	// cout << colorText(colorText("-----------------------------------------------------------", 5), 92) << endl;
	std::cout << std::endl;
	std::cout << colorText("-----------------------------------------------------------", 90) << std::endl;
}

struct ofTemplate;
struct ofAddon;

struct ofProject {
public:
	fs::path path;
	std::vector<ofAddon *> addons;
	std::vector<ofTemplate *> templates;
	void build();
	void cleanTemplates();
};


inline void replace_all(
	std::string & s,
	std::string const & toReplace,
	std::string const & replaceWith) {
	std::string buf;
	std::size_t pos = 0;
	std::size_t prevPos;

	// Reserves rough estimate of final size of string.
	buf.reserve(s.size());

	while (true) {
		prevPos = pos;
		pos = s.find(toReplace, pos);
		if (pos == std::string::npos)
			break;
		buf.append(s, prevPos, pos - prevPos);
		buf += replaceWith;
		pos += toReplace.size();
	}

	buf.append(s, prevPos, s.size() - prevPos);
	s.swap(buf);
}

inline std::string joinStrings(const std::vector<std::string> & vec, const std::string & delimiter) {
	std::string result = "";
	if (vec.empty()) {
		return result;
	}

	// Append the first element
	result += vec[0];

	// Append remaining elements with the delimiter
	for (size_t i = 1; i < vec.size(); ++i) {
		result += delimiter;
		result += vec[i];
	}
	return result;
}



// ██████ █████ ░░░  ▒  ▒ ▒ ▓    ███  TM
// █    █ █     ░  ░ ▒  ▒ ▒ ▓    █  █
// █    █ ████  ░░░  ▒  ▒ ▒ ▓    █  █
// █    █ █     ░  ░ ▒  ▒ ▒ ▓    █  █
// ██████ █     ░░░  ▒▒▒▒ ▒ ▓▓▓▓ ███
// DIAM FONT

// // ORIGINAL STYLE
//  ▗▄▖ ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖
// ▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌
// ▐▌ ▐▌▐▛▀▀▘▐▌▝▜▌▐▛▀▀▘▐▌ ▝▜▌
// ▝▚▄▞▘▐▌   ▝▚▄▞▘▐▙▄▄▖▐▌  ▐▌
//

//    ____  ___________________   __
//   / __ \/ ____/ ____/ ____/ | / /
//  / / / / /_  / / __/ __/ /  |/ /
// / /_/ / __/ / /_/ / /___/ /|  /
// \____/_/    \____/_____/_/ |_/


//  ░░░░░░  ▒▒▒▒▒▒▒  ▓▓▓▓▓▓  ███████ ███    ██
// ░░    ░░ ▒▒      ▓▓       ██      ████   ██
// ░░    ░░ ▒▒▒▒▒   ▓▓   ▓▓▓ █████   ██ ██  ██
// ░░    ░░ ▒▒      ▓▓    ▓▓ ██      ██  ██ ██
//  ░░░░░░  ▒▒       ▓▓▓▓▓▓  ███████ ██   ████

// ░░░░░░░ ▒▒▒▒▒▒ ▓▓▓▓▓▓▓ ██████ ███    ██
// ░░   ░░ ▒▒     ▓▓      ██     ████   ██
// ░░   ░░ ▒▒▒▒▒  ▓▓  ▓▓▓ █████  ██ ██  ██
// ░░   ░░ ▒▒     ▓▓   ▓▓ ██     ██  ██ ██
// ░░░░░░░ ▒▒     ▓▓▓▓▓▓▓ ██████ ██   ████

// ░░░░░ ▒▒▒▒ ▓▓▓▓▓ ████ ██   █
// ░   ░ ▒    ▓     █    ███  █
// ░   ░ ▒▒▒  ▓  ▓▓ ███  █ █  █
// ░   ░ ▒    ▓   ▓ █    █  █ █
// ░░░░░ ▒    ▓▓▓▓▓ ████ █   ██

//  ░░░  ▒▒▒▒  ▓▓▓  ████ █   █
// ░   ░ ▒    ▓     █    ██  █
// ░   ░ ▒▒▒  ▓  ▓▓ ███  █ █ █
// ░   ░ ▒    ▓   ▓ █    █  ██
//  ░░░  ▒     ▓▓▓  ████ █   █
