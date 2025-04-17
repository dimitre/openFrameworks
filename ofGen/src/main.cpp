#include "addons.h"
#include "utils.h"
#include <chrono>

int main(const int argc, const char * argv[]) {
	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << sign << std::endl; // HEADER
	conf.parseParameters(argc, argv);

	bool build = true;
	if (!empty(conf.singleParameter)) {
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
			}
		} else if (conf.singleParameter == "cleantemplates") {
			// project.eraseTemplates();
		}

		else {
			alert("exiting: invalid parameter " + conf.singleParameter, 95);
			exit(1);
		}
	} else {
		buildProject();
		auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> ms_double = t2 - t1;
		std::cout << "" << ms_double.count() << " seconds" << std::endl;
	}
	// if (build) {
	// 	build = buildProject();
	// }

	std::cout << std::endl;
	alert(getPGVersion(), 92);

	// if (build) {
	// 	auto t2 = std::chrono::high_resolution_clock::now();
	// 	std::chrono::duration<double> ms_double = t2 - t1;
	// 	std::cout << "" << ms_double.count() << " seconds" << std::endl;
	// }
	// std::cout << std::endl;
	return 0;
}
