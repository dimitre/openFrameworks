#include "templates.h"

struct ofTemplateZed : public ofTemplate {
public:
	ofTemplateZed() {
		name = "zed";
		path = conf.ofPath / "scripts" / "templates" / name;

		openCommand = "zed . ";
	}
	void load() override;
	void save() override;
};
