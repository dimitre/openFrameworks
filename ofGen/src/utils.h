// void ofStringReplace(string & strIn, const string & from, const string & to) {
//     strIn = std::regex_replace(strIn, std::regex(from), to);
// }

#pragma once

#include <iostream> // cout
#include <map>
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
// namespace fs = std::filesystem;
namespace fs = std::__fs::filesystem;

// #include "functions.h"

std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to);

std::vector<std::string> textToVector(const fs::path & file);

inline std::string colorText(const std::string & s, int color) {
	std::string c { std::to_string(color) };
	return "\033[1;" + c + "m" + s + "\033[0m";
}

inline void alert(std::string msg, int color = 33) {
	std::cout << colorText(msg, color) << std::endl;
}

// DIAM FONT
const std::string sign = colorText(R"(
 ▗▄▖ ▗▄▄▄▖ ▗▄▄▖▗▄▄▄▖▗▖  ▗▖
▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▛▚▖▐▌
▐▌ ▐▌▐▛▀▀▘▐▌▝▜▌▐▛▀▀▘▐▌ ▝▜▌
▝▚▄▞▘▐▌   ▝▚▄▞▘▐▙▄▄▖▐▌  ▐▌
                Prototype 0.01⚡️
)",
							 91)

	+ colorText(R"(                Report issues on
                https://github.com/dimitre/ofLibs/
)",
		92)
	+
	R"(
Now it is only possible to create projects inside
OF installation, three folders up. ex: of/apps/myApps/transcendence
to create a project there, first create the folder,
cd to the folder and invoke ofGen

)";

static void divider() {
	// cout << colorText(colorText("-----------------------------------------------------------", 5), 92) << endl;
	std::cout << colorText("-----------------------------------------------------------", 92) << std::endl;
}

static void replaceAll(std::string & str, const std::string & from, const std::string & to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

// maybe not needed. replace by a normal split string.
inline std::vector<std::string> splitStringOnceByLeft(const std::string & source, const std::string & delimiter) {
	size_t pos = source.find(delimiter);
	std::vector<std::string> res;
	if (pos == std::string::npos) {
		res.emplace_back(source);
		return res;
	}

	res.emplace_back(source.substr(0, pos));
	res.emplace_back(source.substr(pos + delimiter.length()));
	return res;
}

// {
//     return std::regex_replace(strIn, std::regex(from), to);
// }



// trim from start (in place)
inline void ltrim(std::string & s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
inline void rtrim(std::string & s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(),
		s.end());
}

inline std::string ofTrim(std::string line) {
	rtrim(line);
	ltrim(line);
	// line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());
	return line;
}
