
// // #if __has_include(<filesystem>)
// #include <filesystem>
// namespace fs = std::filesystem;
// // #else
// // 	#include <experimental/filesystem>
// // 	namespace fs = std::__fs::filesystem;
// // #endif

#include <fstream> // ifstream
#include <iostream> // cout
#include <regex>
#include <vector>

#include "addons.h"
#include "utils.h"

std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
	return std::regex_replace(strIn, std::regex(from), to);
}

bool ofIsPathInPath(const fs::path & path, const fs::path & base) {
	auto rel = fs::relative(path, base);
	return !rel.empty() && rel.native()[0] != '.';
}
// std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
// 	return std::regex_replace(strIn, std::regex(from), to);
// }

std::vector<std::string> textToVector(const fs::path & file) {
	std::vector<std::string> out;
	if (fs::exists(file)) {
		std::ifstream thisFile(file);
		std::string line;
		while (getline(thisFile, line)) {
			out.emplace_back(line);
		}
	}
	return out;
}

void replaceAll(std::string & str, const std::string & from, const std::string & to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

void ltrim(std::string & s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
	}));
}

// trim from end (in place)
void rtrim(std::string & s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
	}).base(),
		s.end());
}

std::string ofTrim(std::string line) {
	rtrim(line);
	ltrim(line);
	// line.erase(std::remove_if( line.begin(), line.end(), ::isspace), line.end());
	return line;
}
