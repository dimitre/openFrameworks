// #include "functions.h"
#include "utils.h"
// extern genConfig conf;

#include "addons.h"

#include <chrono>

int main(const int argc, const char * argv[]) {
	auto t1 = std::chrono::high_resolution_clock::now();

	std::cout << sign << std::endl; // HEADER
	conf.parseParameters(argc, argv);

	// std::cout << "main.cpp &conf" << std::endl;
	// std::cout << &conf << std::endl;
	// for (auto & t : conf.templateNames) {
	// 	alert(t, 95);
	// }

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

	// fs::path configFile = "of.yml";
	// bool hasConfig = false;
	// if (!fs::exists(configFile)) {
	//    alert("missing of.yml file ", 31);
	// } else {
 //    	hasConfig = true;
 //    	YAML::Node config;
 //    	config = YAML::LoadFile(configFile);
 //    	if (config["ofpath"]) {  // use ofpath only if the key exists.
 //    		auto ofPathYML = config["ofpath"];
 //    		conf.ofPath = ofPathYML.as<string>();
 //    	}

	// }


	if (build) {
		if (!conf.isValidOfPath()) {
			alert("OF not found in default path " + conf.ofPath.string());
			conf.help();
		} else {
			alert("of path OK, proceeding");
			gatherProjectInfo();
		}

		if (conf.singleParameter == "open") {
			conf.open();
		}
		if (conf.singleParameter == "build") {
			conf.build();
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
