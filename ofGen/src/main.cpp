#include "addons.h"
#include "utils.h"
#include <chrono>

#if defined(_WIN32)
	#include <windows.h>
#endif

int main(const int argc, const char * argv[]) {
	auto t1 = std::chrono::high_resolution_clock::now();


#if defined(_WIN32)
	SetConsoleOutputCP(CP_UTF8); // 65001
#endif
	conf.parseParameters(argc, argv);
	if (conf.singleParameter == "yaml-addons-ls") {

		auto addonsFolder { conf.ofPath / "addons" };
		if (fs::exists(addonsFolder)) {
			YAML::Node addonsList(YAML::NodeType::Sequence);
			for (auto const & d : fs::directory_iterator { addonsFolder }) {
				if (fs::is_directory(d.path())) {
					addonsList.push_back(d.path().filename().string());
				}
			}
			std::cout << addonsList << std::endl;
		}
		std::exit(0);
	}

	std::cout << sign << std::endl; // HEADER

	bool build = true;

	if (!fs::exists("src") && !fs::exists("of.yml") && !fs::exists("addons.make") && !fs::exists("../../../.ofroot")) {
		alert("⚠️ Not an ofWorks project folder, no action taken", 95);
		alert("no src folder found, no of.yml file or addons.make found and no OF installed in default path ../../..");
		build = false;
		conf.help();
	}

	if (!empty(conf.singleParameter)) {
		alert("single parameter: " + conf.singleParameter);
		build = false;

		// First parameters without bulding project.
		if (conf.singleParameter == "colors") {
			testColors();
		} else if (conf.singleParameter == "import") {
			conf.import();
		} else if (conf.singleParameter == "help") {
			conf.help();
		}
		// Now building projects
		else if (conf.singleParameter == "open") {
			buildProject();
			conf.open();
		} else if (conf.singleParameter == "build") {
			buildProject();
			conf.build();
		} else if (conf.singleParameter == "buildrun") {
			buildProject();
			int result = conf.build();
			// cout << "OWWWW " << x << endl;
			if (result == 0) {
				conf.run();
			} else {
				std::cerr << "Fail with error " << result << std::endl;
			}
		}

		// FIXME: finish this way of displaying the command to build and run to be used in bash
		else if (conf.singleParameter == "echobuildrun") {
			buildProject();
			cout << conf.buildCommand << endl;
			exit(1);
		} else if (conf.singleParameter == "cleantemplates") {
			conf.project.cleanTemplates();
		}

		else {
			alert("exiting: invalid parameter " + conf.singleParameter, 95);
			exit(1);
		}
	} else {
		if (build) {
			buildProject();
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

	// if (build) {
	// 	auto t2 = std::chrono::high_resolution_clock::now();
	// 	std::chrono::duration<double> ms_double = t2 - t1;
	// 	std::cout << "" << ms_double.count() << " seconds" << std::endl;
	// }
	// std::cout << std::endl;
	return 0;
}
