#include "templates.h"
struct genConfig;
extern genConfig conf;

struct ofTemplateZed : public ofTemplate {
public:
	ofTemplateZed();
	void load() override;
	void save() override;
};
