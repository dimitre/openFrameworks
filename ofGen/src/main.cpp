// #include "functions.h"
#include "utils.h"
// extern genConfig conf;

#include "addons.h"

#include <chrono>

int main(const int argc, const char * argv[]) {
	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << sign << std::endl; // HEADER
	conf.parseParameters(argc, argv);

	bool build = true;

	if (conf.singleParameter == "colors") {
		testColors();
		build = false;
	}

	else if (conf.singleParameter == "import") {
		conf.import();
		build = false;
	}

	else if (conf.singleParameter == "help") {
		conf.help();
		build = false;
	}


	if (build) {
		gatherProjectInfo();

		// if (!conf.isValidOfPath()) {
		// 	alert("OF not found in default path " + conf.ofPath.string());
		// 	conf.help();
		// } else {
		// 	alert("of path OK, proceeding");
		// 	gatherProjectInfo();
		// }

		if (conf.singleParameter == "open") {
			conf.open();
		}
		if (conf.singleParameter == "build") {
			conf.build();
		}

		else if (conf.singleParameter == "buildrun") {
    		conf.build();
            conf.run();
		}
	}


	std::cout << std::endl;
	alert(getPGVersion(), 92);

	if (build) {
		auto t2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> ms_double = t2 - t1;
		std::cout << "" << ms_double.count() << " seconds" << std::endl;
	}
	std::cout << std::endl;
	return 0;
}
