// #include "functions.h"
// #include "utils.h"
#include "addons.h"

#include <chrono>


int main(const int argc, const char * argv[]) {
    auto t1 = std::chrono::high_resolution_clock::now();

	// testColors();
	cout << sign << endl; // HEADER
	parseParameters(argc, argv);

	if (!isValidOfPath()) {
		alert("invalid of path");
		conf.help();
		std::exit(1);
	} else {
		alert("of path OK, proceeding");
	}
	// if (argc > 1) {
	// 	string param = argv[1];
	// }

	gatherProjectInfo();
	// parseConfigAllAddons();
	// createTemplates();
	// infoTemplates();
	// buildProject();
	auto t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> ms_double = t2 - t1;

	std::cout << std::endl;
	std::cout << ms_double.count() << " seconds" << std::endl;
	return 0;
}
