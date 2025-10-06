#include "addons.h"
#include "utils.h"
#include <chrono>

int main(const int argc, const char * argv[]) {
	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << sign << std::endl; // HEADER
	conf.parseParameters(argc, argv);

	bool build = true;

	if (!fs::exists("src") && !fs::exists("of.yml") && !fs::exists("addons.make") && !fs::exists("../../../.ofroot")) {
		alert("no src folder found, no of.yml file or addons.make found and no OF installed in default path ../../..", 95);
		build = false;
		conf.help();
	}

	if (!empty(conf.singleParameter)) {
		alert("this is single parameter " + conf.singleParameter, 96);
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
			// project.eraseTemplates();
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
