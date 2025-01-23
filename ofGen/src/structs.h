

struct ofAddon {
public:
    string name;
    fs::path path;
    std::map<string, vector<string> > addonProperties;
};

struct ofTemplate {
    public:
    string name { "" };
    fs::path path;
    ofTemplate() { }
    vector <copyTemplateFile> copyTemplateFiles;
    virtual void load() {}
    virtual void build() {
        for (auto & c : copyTemplateFiles) {
            c.run();
        }
    }

    // load to memory
};

struct ofTemplateMacos : public ofTemplate {
    public:
    ofTemplateMacos() {
        name = "macos";
        path = conf.ofPath / "scripts" / "templates" / name;
    }
    void load() override {};
    void build() override {};
};

struct ofTemplateZed : public ofTemplate {
    public:
    ofTemplateZed() {
        name = "zed";
        path = conf.ofPath / "scripts" / "templates" / name;
    }
    void load() override {};
    void build() override {};
};

vector <ofAddon> ofAddons;
vector <ofTemplate*> ofTemplates;



struct ofProject {
public:
    vector <ofAddon*> addonsPointer;
    vector <ofTemplate*> templates;

    void build() {
        cout << "addonsPointer.size " << addonsPointer.size() << endl;
        cout << "templates.size " << templates.size() << endl;
    }
};
