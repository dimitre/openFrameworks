#include "templates.h"
#include <fstream>

struct ofTemplateMake : public ofTemplate {
public:
	ofTemplateMake() {
		name = "make";
		path = conf.ofPath / "scripts" / "templates" / name;

		buildCommand = "make -j";
		runCommand = "make RunRelease";
		cleanCommand = "make clean";
	}
	void load() override;
	void save() override;
};
