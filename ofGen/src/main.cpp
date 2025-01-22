#include "functions.h"

void testColors() {
    /*
    Color: 5 = blink white
    Color 7 : invert background
    30 : preto 31, 36 - cores
    41, 47 cores de fundo
    91/96 : cores vivas
    100/107 : cores vivas fundo

    */
    int colors[] = {
        // 5, 7, 30, 31, 32, 33, 34, 35, 36,
        91, 92, 93, 94, 95, 96,
        // 41, 42, 43, 44, 45, 46, 47,
        // 100, 101, 102, 103, 104, 105, 106, 107,
    };

    for (auto & a : colors ){
        cout << colorText ("████ " + std::to_string(a), a) ;
    }
    cout << endl;
}

#define VERSION "Build System for OpenFrameworks v0.1.0"
const fs::path templatesFolder = "scripts/templates";

int main(const int argc, const char* argv[]) {
    // HEADER
    cout << sign << endl;
    cout << "current folder: " << fs::current_path() << endl;

    createMacosProject();
    createTemplates();

    if (fs::exists(conf.ofPath / ".ofroot")) {
        alert("ofPath valid, proceeding", 93);
    } else {
        cout << "ofPath not found: edit ofPath? (y)es, (n)o, (q)uit" << endl;
        std::exit(0);
    }

    if (fs::exists("of.yml")) { // configuration found
        // start building with of.yml
    } else {
        if (fs::exists("addons.make")) {
            cout << "addons.make found. importing addons list from there" << endl;
            // import addons.make
        } else {
            cout << "Both of.yml and addons.make not found. Let's create a configuration here" << endl;
            // create configuration.
            // having getPlatformString() selected by default.
        }
    }

    // alert("█", 5); //blinking cursor
    // cout << "ok 23 " << templatesFolder << endl;
    // parseConfig();

    // if (fs::exists(conf.ofPath / ".ofroot")) {
    //     alert("valid ofPath, great stuff", 93);
    // } else {
    //     alert("invalid ofPath, I'm out", 94);
    // }

   	if (argc == 1) {
        alert ("ofGen with no parameters. will create project on this folder and consider of path = ../../..");
		// build.load();
		alert ("platform not passed as a parameter, will generate as " + getPlatformString() , 33);
	}
    return 0;
}
