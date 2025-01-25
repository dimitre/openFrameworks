
// #if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
// #else
// 	#include <experimental/filesystem>
// 	namespace fs = std::__fs::filesystem;
// #endif

#include <fstream> // ifstream
#include <iostream> // cout
#include <regex>
#include <vector>

#include "utils.h"

std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
	return std::regex_replace(strIn, std::regex(from), to);
}

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
