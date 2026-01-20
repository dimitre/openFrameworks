#include "templates.h"
struct genConfig;
extern genConfig conf;

struct ofTemplateMake : public ofTemplate {
public:
	ofTemplateMake();
	void load() override;
	void save() override;
};
