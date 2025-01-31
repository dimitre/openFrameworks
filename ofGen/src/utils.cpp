
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

// #include "addons.h"
#include "utils.h"

genConfig conf;

std::string ofPathToString(const fs::path & path) {
	try {
		return path.string();
	} catch (fs::filesystem_error & e) {
		std::cerr << "ofPathToString: error converting fs::path to string " << e.what();
	}
	return {};
}

std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
	return std::regex_replace(strIn, std::regex(from), to);
}

bool ofIsPathInPath(const fs::path & path, const fs::path & base) {
	auto rel = fs::relative(path, base);
	// cout << "ofIsPathInPath " << rel << endl;
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

std::vector<std::string> ofSplitString(const std::string& s, const std::string& delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}
