
#define VERSION "Build System for OpenFrameworks v0.1.0"

#include <iostream>
#if __has_include(<filesystem>)
	#include <filesystem>
#else
	#include <experimental/filesystem>
#endif
#include <vector>

using std::cout;
using std::endl;
using std::string;
using std::vector;

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <yaml-cpp/yaml.h>
// const fs::path templatesFolder = "scripts/templates";

int main(const int argc, const char* argv[]) {
    cout << "ok" << endl;
    return 0;
}
/*
we now have platforms (operational systems?) (windows, linux)
we have targets (ios, macos, android etc.)
we have IDEs tied to platforms (xcode, visual studio)
and IDEs attending multiple platforms/targets (VSCode, ZED)
and build systems (Make) attending multiple platforms/targets
 */
