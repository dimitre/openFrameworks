// #include "functions.h"
// #include "utils.h"
#include "addons.h"

int main(const int argc, const char * argv[]) {

	testColors();
	cout << sign << endl; // HEADER
	// fs::current_path("../apps/werkApps/Pulsar");

	// we have parameters here.
	// ./ofGen platforms=zed,macos addons=ofxMicroUI,ofxTools ofroot=../../.. path=.

	parseParameters(argc, argv);

	if (!isValidOfPath()) {
		alert("invalid of path");
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
	return 0;
}
