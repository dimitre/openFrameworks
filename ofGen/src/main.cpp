#include "functions.h"
#define VERSION "Build System for OpenFrameworks v0.1.0"

const fs::path templatesFolder = "scripts/templates";


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

int main(const int argc, const char* argv[]) {
    cout << "ok 23 " << templatesFolder << endl;

    // string s = "lara x lara";
    // alert(s, 31);
    // ofStringReplace(s, "lara", "cucuio");
    // alert(s, 32);
    // std::exit(1);
    for (auto & a : colors ){
        cout << colorText ("████ " + std::to_string(a), a) ;
    }
    cout << endl;

    cout << sign << endl;
    parseConfig();

    if (fs::exists(conf.ofPath / ".ofroot")) {
        alert("valid ofPath, great stuff", 93);
    } else {
        alert("invalid ofPath, I'm out", 94);
        std::exit(0);
    }

   	if (argc == 1) {
        alert ("ofGen with no parameters. will create project on this folder and consider of path = ../../..");
		// build.load();
		alert ("platform not passed as a parameter, will generate as " + getPlatformString() , 33);
	}
    return 0;
}
