#include "addons.h"
#include "utils.h"
#include <chrono>

#if defined(_WIN32)
	#include <windows.h>
#endif

#include "genConfig.h"

const std::string sign = colorText(R"(
 ░░░  ▒▒▒▒  ▓▓▓  ████ █   █ 〇
░   ░ ▒    ▓     █    ██  █
░   ░ ▒▒▒  ▓  ▓▓ ███  █ █ █
░   ░ ▒    ▓   ▓ █    █  ██
 ░░░  ▒     ▓▓▓  ████ █   █
)",
							 32) // 91
	+ R"(
Project Generator for ofWorks (OpenFrameworks fork)
                 Prototype )"
	+ version

	+ colorText(R"(
                 Report issues or suggestions on
                 https://github.com/ofworks/ofworks/
                 https://ofworks.cc/
)",
		34); // 92

// )";

inline void testColors() {
	/*
    Color: 5 = blink white
    Color 7 : invert background
    30 : preto 31, 36 - cores
    41, 47 cores de fundo
    91/96 : cores vivas
    100/107 : cores vivas fundo

    */
	int colors[] = {
		// 5, //blinking
		0, 2, 5, 7, 30, 31, 32, 33, 34, 35, 36,
		90, 91, 92, 93, 94, 95, 96,
		// 41, 42, 43, 44, 45, 46, 47,
		// 100, 101, 102, 103, 104, 105, 106, 107,
	};

	cout << endl;
	for (auto & a : colors) {
		cout << colorText(std::to_string(a) + "███  ", a);
	}
	cout << endl;
	cout << endl;
}

int main(const int argc, const char * argv[]) {
	auto t1 = std::chrono::high_resolution_clock::now();

#if defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8); // 65001
#endif

	conf.parseParameters(argc, argv);

	if (conf.singleParameter == "--version") {
		// conf.version();
		// cout << getVersion() << endl;
		cout << version << endl;
		std::exit(0);
	} else if (conf.singleParameter == "yaml-addons-ls") {
		// Pure Yaml output goes here, before the header SIGN
		conf.listAddonsAsYaml();
		std::exit(0);
	}

	std::cout << sign << std::endl; // HEADER
	bool build = true;

	if (!empty(conf.singleParameter)) {
		alert("single parameter: " + conf.singleParameter);
		build = false;

		// First parameters without bulding project.
		if (conf.singleParameter == "rm") {
			if (fs::exists("of.yml")) {
				std::vector<std::string> lines {
					"rm .DS_Store",
					"rm addons.make",

					"rm .chaletrc",
					"rm chalet.yaml",
					"rm compile_flags.txt",
					"rm -rf build",

					// MAKE
					"rm config.make",
					"rm Makefile",

					// XCODE
					"rm of.entitlements",
					"rm openFrameworks-Info.plist",
					"rm App.xcconfig",
					"rm Project.xcconfig",
					"rm -rf *.xcodeproj",

					"rm -rf .vscode",
					"rm -rf .zed",
					"rm -rf .cache",

					// VisualStudio
					"rm *.sln",
					"rm *.vcxproj",
					"rm *.filters",
					"rm *.user",
					"rm icon.rc",
				};

				for (const auto & l : lines) {
					std::system(l.c_str());
				}
				std::exit(0);

			} else {
				alert("no of.yml present, exiting", 96);
				std::exit(0);
			}
		}

		if (conf.singleParameter == "colors") {
			testColors();
		} else if (conf.singleParameter == "import") {
			conf.import();
		} else if (conf.singleParameter == "help") {
			conf.help();
		}
		// Now building projects
		else if (conf.singleParameter == "bundle" || conf.singleParameter == "dist") {
			conf.buildProject();
			conf.bundleProject();
			conf.dist();
		} else if (conf.singleParameter == "open") {
			conf.buildProject();
			conf.open();
		} else if (conf.singleParameter == "build") {
			conf.buildProject();
			conf.build();
		} else if (conf.singleParameter == "bump") {
			conf.bump();
			// conf.buildProject();
			// conf.build();
		} else if (conf.singleParameter == "buildrun") {
			conf.buildProject();
			int result = conf.build();
			// cout << "OWWWW " << x << endl;
			if (result == 0) {
				conf.run();
			} else {
				std::cerr << "Fail with error " << result << std::endl;
			}
		}

		// FIXME: finish this way of displaying the command to build and run to be used in bash
		// else if (conf.singleParameter == "echobuildrun") {
		// 	conf.buildProject();
		// 	for (auto & t : conf.templates) {
		// 		if (t->commands.count("build")) {
		// 			cout << t->commands["build"] << endl;
		// 			break;
		// 		}
		// 	}
		// 	exit(1);
		// }

		else if (conf.singleParameter == "cleantemplates") {
			conf.project.cleanTemplates();
		}

		else {
			alert("exiting: invalid parameter " + conf.singleParameter, 95);
			exit(1);
		}
	} else {
		if (build) {
			conf.buildProject();
			auto t2 = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> ms_double = t2 - t1;
			std::cout << "" << ms_double.count() << " seconds" << std::endl;
		}
	}
	// if (build) {
	// 	build = buildProject();
	// }

	std::cout << std::endl;
	alert(getVersion(), 92);
	alert("https://ofworks.cc", 94);
	std::cout << std::endl;

	// if (build) {
	// 	auto t2 = std::chrono::high_resolution_clock::now();
	// 	std::chrono::duration<double> ms_double = t2 - t1;
	// 	std::cout << "" << ms_double.count() << " seconds" << std::endl;
	// }
	// std::cout << std::endl;
	return 0;
}
