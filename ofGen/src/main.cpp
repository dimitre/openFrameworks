#include "functions.h"

int main(const int argc, const char * argv[]) {
	// HEADER
	cout << sign << endl;
	cout << "current folder: " << fs::current_path() << endl;

	createMacosProject();
	// createTemplates();

	// if (fs::exists(conf.ofPath / ".ofroot")) {
	//     alert("ofPath valid, proceeding", 93);
	// } else {
	//     cout << "ofPath not found: edit ofPath? (y)es, (n)o, (q)uit" << endl;
	//     std::exit(0);
	// }

	// if (fs::exists("of.yml")) { // configuration found
	//     // start building with of.yml
	// } else {
	//     if (fs::exists("addons.make")) {
	//         cout << "addons.make found. importing addons list from there" << endl;
	//         // import addons.make
	//     } else {
	//         cout << "Both of.yml and addons.make not found. Let's create a configuration here" << endl;
	//         // create configuration.
	//         // having getPlatformString() selected by default.
	//     }
	// }

	// alert("█", 5); //blinking cursor
	// cout << "ok 23 " << templatesFolder << endl;
	// parseConfig();

	// if (fs::exists(conf.ofPath / ".ofroot")) {
	//     alert("valid ofPath, great stuff", 93);
	// } else {
	//     alert("invalid ofPath, I'm out", 94);
	// }

	//   	if (argc == 1) {
	//        alert ("ofGen with no parameters. will create project on this folder and consider of path = ../../..");
	// 	// build.load();
	// 	alert ("platform not passed as a parameter, will generate as " + getPlatformString() , 33);
	// }
	return 0;
}
