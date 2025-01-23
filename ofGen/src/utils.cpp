
#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
// namespace fs = std::filesystem;
namespace fs = std::__fs::filesystem;

#include <regex>
#include <fstream> // ifstream
#include <vector>
#include <iostream> // cout

#include "utils.h"

std::string stringReplace(const std::string & strIn, const std::string & from, const std::string & to) {
    return std::regex_replace(strIn, std::regex(from), to);
}

std::vector<std::string> textToVector (const fs::path & file) {
	std::vector<std::string> out;
	if (fs::exists(file)) {
		std::ifstream thisFile(file);
		std::string line;
		while(getline(thisFile, line)){
			out.emplace_back(line);
		}
	}
	return out;
}


bool copyTemplateFile::run() {

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
				std::cout << "└─ Replacing " << f.first << " : " << f.second << std::endl;
			}

			for (auto & a : appends) {
			     // alert(a, 35);
			    contents += "\n" + a;
			}

			std::ofstream fileTo(to);
			try{
				fileTo << contents;
			}catch(std::exception & e){
				std::cerr << "Error saving to " << to << std::endl;
				std::cerr << e.what() << std::endl;
				return false;
			}catch(...){
				std::cerr << "Error saving to " << to << std::endl;

				return false;
			}


		} else {
			// straight copy
			try {
				fs::copy(from, to, fs::copy_options::update_existing);
			}
			catch(fs::filesystem_error & e) {
				std::cerr << "error copying template file " << from << " : " << to << std::endl;
				std::cerr << e.what() << std::endl;
				return false;
			}
		}
	} else {
		return false;
	}
	return true;
	}
