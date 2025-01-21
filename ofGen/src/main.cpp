
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

// namespace fs = std::filesystem;
namespace fs = std::__fs::filesystem;

const fs::path templatesFolder = "scripts/templates";

int main(const int argc, const char* argv[]) {
    cout << "ok 213 " << templatesFolder << endl;
    return 0;
}
