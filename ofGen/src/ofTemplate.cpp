#include "ofTemplate.h"
#include "utils.h"
#include <fstream> //in utils


void copyTemplateFile::info() {
	alert("	copyTemplateFile", 96);
	alert("	from " + from.string(), 2);
	alert("	to " + to.string(), 90);
	for (auto & f : findReplaces) {
		if (!empty(f.first)) {
			alert("	└─ Replacing " + f.first + " : " + f.second, 94);
			// std::cout << "	└─ Replacing " << f.first << " : " << f.second << std::endl;
		}
	}
}

void copyTemplateFile::load() {
	if (fs::exists(from)) {
		isLoaded = true;
		std::ifstream fileFrom(from);
		// std::string contents((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
		contents = std::string((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
		fileFrom.close();
	}
}

bool copyTemplateFile::run() {
	info();
	if (fs::exists(from)) {

		if (findReplaces.size() || appends.size()) { // || transform != nullptr
			// Load file, replace contents, append data to content, make transformation and then write to destination.

			if (!isLoaded) {
				load();
			}
			// std::ifstream fileFrom(from);
			// // std::string contents((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
			// contents = std::string((std::istreambuf_iterator<char>(fileFrom)), std::istreambuf_iterator<char>());
			// fileFrom.close();

			// if (transform != nullptr) {
			// 	(*transform)(contents);
			// }
			// alert("AFTER TRANSFORM", 95);
			// cout << contents << endl;

			for (auto & f : findReplaces) {
				// Avoid processing empty pairs
				if (empty(f.first) && empty(f.second)) {
					continue;
				}
				replaceAll(contents, f.first, f.second);
			}

			for (auto & a : appends) {
				alert("	└─ append " + a, 90);
				contents += "\n" + a;
			}

			std::ofstream fileTo(to);
			try {
				fileTo << contents;
			} catch (std::exception & e) {
				std::cerr << "Error saving to " << to << std::endl;
				std::cerr << e.what() << std::endl;
				return false;
			} catch (...) {
				std::cerr << "Error saving to " << to << std::endl;

				return false;
			}

		} else {
			// no replacements, straight copy
			if (isFolder) {
				// Remove exists? Remove destination folder?
				if (fs::exists(to)) {
					try {
						fs::remove_all(to);
					} catch (fs::filesystem_error & e) {
						std::cerr << "Error removing template folder: " << to << " " << e.what() << std::endl;
						return false;
					}
				}
				try {
					fs::copy(from, to, fs::copy_options::recursive | fs::copy_options::update_existing);
				} catch (fs::filesystem_error & e) {
					std::cerr << "Error copying template files: " << e.what() << std::endl;
					return false;
				}
			} else {
				try {
					fs::copy(from, to, fs::copy_options::update_existing);
				} catch (fs::filesystem_error & e) {
					std::cerr << "error copying template file " << from << " : " << to << std::endl;
					std::cerr << e.what() << std::endl;
					return false;
				}
			}
		}
	}

	else {
		alert("	input file not found " + from.string(), 91);
		return false;
	}
	return true;
}
