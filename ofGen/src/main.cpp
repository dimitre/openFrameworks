// #include "functions.h"
// #include "utils.h"
#include "addons.h"

#include <chrono>

int main(const int argc, const char * argv[]) {
	auto t1 = std::chrono::high_resolution_clock::now();

	cout << sign << endl; // HEADER
	conf.parseParameters(argc, argv);

	bool build = true;

	if (conf.singleParameter == "colors") {
		testColors();
		build = false;
	}

	if (build) {
		if (!isValidOfPath()) {
			alert("OF not found in default path " + conf.ofPath.string());
			conf.help();
		} else {
			alert("of path OK, proceeding");
			gatherProjectInfo();
		}

		if (conf.singleParameter == "open") {
			conf.open();
		}
	}

	std::cout << std::endl;
	alert(getPGVersion(), 92);

	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> ms_double = t2 - t1;
	std::cout << "" << ms_double.count() << " seconds" << std::endl;
	std::cout << std::endl;
	return 0;
}
