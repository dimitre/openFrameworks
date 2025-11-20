#include "templates.h"
// Visual Studio
#include <pugixml.hpp>

// #include <fstream>
struct ofTemplateVisualStudio : public ofTemplate {
public:
	ofTemplateVisualStudio() {
		name = "visualstudio";
		path = conf.ofPath / "scripts" / "templates" / name;
		//FIXME - implement
		openCommand = "open " + conf.projectName + ".xcodeproj";
		// buildCommand = "msbuild";
		buildCommand = "powershell -c \"msbuild\"";
		// Untested
		cleanCommand = "powershell -c \"msbuild *.sln -t:Clean\"";
	}

	// pugi::xml_document filterXmlDoc;
	// pugi::xml_document doc;

	pugi::xml_document filterXml;
	pugi::xml_document projectXml;

	string convertStringToWindowsSeparator(string in) {
		std::replace(in.begin(), in.end(), '/', '\\');
		return in;
	}
	// void load() override;
	// void save() override;

	void load() override {
		alert("ofTemplateVisualStudio::load()", 92);

		std::pair<string, string> replacementsForward, replacementsBack;
		if (!fs::equivalent(conf.ofPath, fs::path { "../../.." })) {
			string relRootWindows { convertStringToWindowsSeparator(conf.ofPath.string()) + "\\" };
			replacementsForward = { "../../../", relRootWindows };
			replacementsBack = { "..\\..\\..\\", relRootWindows };
		} else {
			//		cout << "equivalent to default ../../.." << endl;
		}

		// copyTemplateFiles.push_back({ path / "config.make",
		// 	conf.projectPath / "config.make" });
		// project
		copyTemplateFiles.push_back({ path / "emptyExample.vcxproj",
			conf.projectPath / (conf.projectName + ".vcxproj"),
			{ { "emptyExample", conf.projectName },
				replacementsForward,
				replacementsBack } });
		copyTemplateFiles.back().load(); // try to load from memory pugi
		{
			pugi::xml_parse_result result { projectXml.load_string(copyTemplateFiles.back().contents.c_str()) };
			if (result.status != pugi::status_ok) {
				alert("     problem parsing " + copyTemplateFiles.back().to.string(), 90);
				std::exit(1);
			} else {
				alert("     " + copyTemplateFiles.back().to.string() + " loaded OK!");
			}
		}

		// filters
		copyTemplateFiles.push_back({ path / "emptyExample.vcxproj.filters",
			conf.projectPath / (conf.projectName + ".vcxproj.filters"),
			{ { "emptyExample", conf.projectName },
				replacementsForward,
				replacementsBack } });
		copyTemplateFiles.back().load(); // try to load from memory pugi
		{
			pugi::xml_parse_result result { filterXml.load_string(copyTemplateFiles.back().contents.c_str()) };
			if (result.status != pugi::status_ok) {
				alert("     problem parsing " + copyTemplateFiles.back().to.string(), 90);
				std::exit(1);
			} else {
				alert("     " + copyTemplateFiles.back().to.string() + " loaded OK!");
			}
		}

		// solution
		copyTemplateFiles.push_back({ path / "emptyExample.sln",
			conf.projectPath / (conf.projectName + ".sln"),
			{ { "emptyExample", conf.projectName },
				replacementsForward,
				replacementsBack } });

		// user
		copyTemplateFiles.push_back({ path / "emptyExample.vcxproj.user",
			conf.projectPath / (conf.projectName + ".vcxproj.user"),
			{ { "emptyExample", conf.projectName },
				replacementsForward,
				replacementsBack } });

		// icon
		copyTemplateFiles.push_back({ path / "icon.rc",
			conf.projectPath / "icon.rc",
			{ { "emptyExample", conf.projectName },
				replacementsForward,
				replacementsBack } });
	}

	void save() override {
		alert("ofTemplateVisualStudio::save()", 92);

		{
			std::ostringstream oss;
			projectXml.save(oss); // Save the document to the ostringstream
			copyTemplateFiles[0].contents = oss.str();
		}
		{
			std::ostringstream oss;
			filterXml.save(oss); // Save the document to the ostringstream
			copyTemplateFiles[1].contents = oss.str();
		}
	}

	bool exclusiveAppend(string & values, string item, string delimiter = ";") {
		auto strings = ofSplitString(values, delimiter);
		bool bAdd = true;
		for (size_t i = 0; i < strings.size(); i++) {
			if (strings[i].compare(item) == 0) {
				bAdd = false;
				break;
			}
		}
		if (bAdd == true) {
			// strings.emplace_back(item);
			values += delimiter + item;
			//unsplitString(strings, delimiter);
			return true;
		}
		return false;
	}

	void addToAllNodes(const pugi::xpath_node_set & nodes, string item, string delimiter = ";") {
		for (auto & node : nodes) {
			std::string values = node.node().first_child().value();
			if (exclusiveAppend(values, item, delimiter)) {

				node.node().first_child().set_value(values.c_str());
				// if(bPrint) {
				// 	string msg = "Adding To Node: " + string(node.node().first_child().value());
				// 	alert(msg, 35);
				// }
			}
			// std::vector < std::string > strings = ofSplitString(includes, delimiter);
			// bool bAdd = true;
			// for (size_t i = 0; i < strings.size(); i++) {
			// 	if (strings[i].compare(item) == 0) {
			// 		bAdd = false;
			// 		break;
			// 	}
			// }
			// if (bAdd == true) {
			// 	strings.emplace_back(item);
			// 	node.node().first_child().set_value(unsplitString(strings, delimiter).c_str());
			// }
		}
	}

	void addInclude(const fs::path & includeName) {
		alert("visualStudioProject::addInclude " + includeName.string(), 35);
		pugi::xpath_node_set source = projectXml.select_nodes("//ClCompile/AdditionalIncludeDirectories");
		addToAllNodes(source, includeName.string());
	}

	void addProps(fs::path propsFile) {
		//	alert ("visualStudioProject::addProps " + propsFile.string());
		string path = ofPathToString(propsFile); //.string();
		//	fixSlashOrder(path);
		pugi::xpath_node_set items = projectXml.select_nodes("//ImportGroup");
		for (auto & item : items) {
			// FIXME: needed?
			pugi::xml_node additionalOptions;
			item.node().append_child("Import").append_attribute("Project").set_value(path.c_str());
		}
		//	auto check = doc.select_nodes("//ImportGroup/Import/Project");
	}

#ifdef PORT
	void addLibrary(const LibraryBinary & lib) {

		auto libraryName = fs::path { lib.path };
		auto libFolder = libraryName.parent_path();
		string libFolderString = libFolder.string();
		// fixSlashOrder(libFolderString);
		auto libName = libraryName.filename();

		// Determine the correct link path based on the target and architecture
		string linkPath;
		if (!lib.target.empty() && !lib.arch.empty()) {
			if (lib.arch == "ARM64") {
				// For ARM64, ensure it does not match ARM64EC
				linkPath = "//ItemDefinitionGroup[contains(@Condition,'" + lib.target + "') and contains(@Condition,'ARM64') and not(contains(@Condition,'ARM64EC'))]/Link/";
			} else {
				// For other architectures
				linkPath = "//ItemDefinitionGroup[contains(@Condition,'" + lib.target + "') and contains(@Condition,'" + lib.arch + "')]/Link/";
			}
		} else if (!lib.target.empty()) {
			linkPath = "//ItemDefinitionGroup[contains(@Condition,'" + lib.target + "')]/Link/";
		} else if (!lib.arch.empty()) {
			if (lib.arch == "ARM64") {
				linkPath = "//ItemDefinitionGroup[contains(@Condition,'ARM64') and not(contains(@Condition,'ARM64EC'))]/Link/";
			} else {
				linkPath = "//ItemDefinitionGroup[contains(@Condition,'" + lib.arch + "')]/Link/";
			}
		} else {
			linkPath = "//ItemDefinitionGroup/Link/";
		}

		// Add library paths and names to the correct ItemDefinitionGroup based on the link path
		if (!libFolderString.empty()) {
			pugi::xpath_node_set addlLibsDir = doc.select_nodes((linkPath + "AdditionalLibraryDirectories").c_str());
			ofLogVerbose() << "adding " << lib.arch << " lib path " << linkPath;
			addToAllNodes(addlLibsDir, libFolderString);
		}

		pugi::xpath_node_set addlDeps = doc.select_nodes((linkPath + "AdditionalDependencies").c_str());
		addToAllNodes(addlDeps, libName.string());

		ofLogVerbose("visualStudioProject::addLibrary") << "adding lib path " << libFolder;
		ofLogVerbose("visualStudioProject::addLibrary") << "adding lib " << libName;
	}

	// FIXME libtype
	void addCompileOption(const string & nodeName, const string & value, const string & delimiter, LibType libType) {
		string configuration = ((libType == DEBUG_LIB) ? "Debug" : "Release");
		string nodePath = "//ItemDefinitionGroup[contains(@Condition,'" + configuration + "')]/ClCompile/" + nodeName;
		pugi::xpath_node_set source = projectXml.select_nodes(nodePath.c_str());
		addToAllNodes(source, value, delimiter);
	}

	void addCFLAG(const string & cflag, LibType libType) {
		addCompileOption("AdditionalOptions", cflag, " ", libType);
	}

	void addCPPFLAG(const string & cppflag, LibType libType) {
		addCompileOption("AdditionalOptions", cppflag, " ", libType);
	}

	void addDefine(const string & define, LibType libType) {
		addCompileOption("PreprocessorDefinitions", define, ";", libType);
	}

	void ensureDllDirectoriesExist() {
		std::vector<fs::path> dirs { { "dll/x64" }, { "dll/ARM64" }, { "dll/ARM64EC" } };
		for (const auto & dir : dirs) {
			fs::path dirPath = { projectDir / dir };
			dirPath = normalizePath(dirPath);
			if (!fs::exists(dirPath)) {
				ofLogVerbose() << "adding dll folder: [" << dirPath.string() << "]";
				try {
					fs::create_directories(dirPath);
				} catch (const std::exception & e) {
					std::cerr << "Error creating directories: " << e.what() << std::endl;
				}
			}
		}
	}

	void addAddonIncludePaths(const ofAddon & addon) {

		std::set<fs::path> uniqueIncludeDirs;
		for (const auto & dir : addon.includePaths) {
			fs::path normalizedDir = normalizePath(dir);
			std::string dirStr = normalizedDir.string();
			// this dont work
			if (dirStr.find("lib\\vs") == std::string::npos && dirStr.find("\\license") == std::string::npos && dirStr.find("lib\\\\vs") == std::string::npos && dirStr.find("lib\\AndroidJNI") == std::string::npos && dirStr.find("\\bin\\") == std::string::npos) {
				uniqueIncludeDirs.insert(normalizedDir);
			} else {
				ofLogVerbose() << "include dir - not adding vs: [" << dir.string() << "]";
			}
		}

		for (const auto & dir : uniqueIncludeDirs) {
			if ((dir.string().size() && dir.string()[0] == '$')) {
				addInclude(dir.string());
			} else {
				fs::path normalizedDir = normalizePath(dir);
				normalizedDir = makeRelative(projectDir, dir);
				ofLogVerbose() << "[vsproject]-uniqueIncludeDirs] Adding dir:: [" << normalizedDir.string() << "]";
				addInclude(normalizedDir);
			}
		}
	}

	void visualStudioProject::addAddonCflags(const ofAddon & addon) {
		for (auto & a : addon.cflags) {
			ofLogVerbose() << "Adding addon CFLAG: [" << a << "]";
			addCFLAG(a, RELEASE_LIB);
			addCFLAG(a, DEBUG_LIB);
		}
	}
	void visualStudioProject::addAddonCppflags(const ofAddon & addon) {
		for (auto & a : addon.cppflags) {
			ofLogVerbose() << "Adding addon CPPFLAG: [" << a << "]";
			addCPPFLAG(a, RELEASE_LIB);
			addCPPFLAG(a, DEBUG_LIB);
		}
	}

	void visualStudioProject::addSrcFiles(ofAddon & addon, const vector<fs::path> & filepaths, SrcType type, bool bFindInFilesToFolder) {
		for (auto & s : filepaths) {

			if (bFindInFilesToFolder && (addon.filesToFolders.find(s) == addon.filesToFolders.end())) {
				addon.filesToFolders[s] = fs::path { "" };
			}
			ofLogVerbose("visualStudioProject::addSrcFiles") << "Adding addon " << toString(type) << " source file: [" << s.string() << "] folder:[" << addon.filesToFolders[s].string() << "]";
			addSrc(s, addon.filesToFolders[s]);
		}
	}

	void visualStudioProject::addAddonDefines(const ofAddon & addon) {
		for (auto & a : addon.defines) {
			ofLogVerbose() << "Adding addon define: [" << a << "]";
			addDefine(a, RELEASE_LIB);
			addDefine(a, DEBUG_LIB);
		}
	}

	void visualStudioProject::addAddonProps(const ofAddon & addon) {
		// Add props files from the addon
		for (auto & props : addon.propsFiles) {
			fs::path normalizedDir = makeRelative(projectDir, props);
			ofLogVerbose() << "Adding addon props: [" << normalizedDir.string() << "] folder:[" << addon.filesToFolders.at(props).string() << "]";
			addProps(normalizedDir);
		}
	}
#endif

	void addAddon(ofAddon * a) override;

	void appendFilter(string folderName) {
		string uuid { generateUUID(folderName) };
		string tag { "//ItemGroup[Filter]/Filter[@Include=\"" + folderName + "\"]" };
		pugi::xpath_node_set set = filterXml.select_nodes(tag.c_str());
		if (set.size() > 0) {
			//pugi::xml_node node = set[0].node();
		} else {

			pugi::xml_node node = filterXml.select_node("//ItemGroup[Filter]/Filter").node().parent();
			pugi::xml_node nodeAdded = node.append_child("Filter");
			nodeAdded.append_attribute("Include").set_value(folderName.c_str());
			pugi::xml_node nodeAdded2 = nodeAdded.append_child("UniqueIdentifier");

			uuid.insert(8, "-");
			uuid.insert(8 + 4 + 1, "-");
			uuid.insert(8 + 4 + 4 + 2, "-");
			uuid.insert(8 + 4 + 4 + 4 + 3, "-");

			//d8376475-7454-4a24-b08a-aac121d3ad6f

			string uuidAltered = "{" + uuid + "}";
			nodeAdded2.append_child(pugi::node_pcdata).set_value(uuidAltered.c_str());
		}
	}

	bool doesTagAndAttributeExist(pugi::xml_document & doc, string tag, string attribute, string newValue) {
		string expression { "//" + tag + "[@" + attribute + "='" + newValue + "']" };
		pugi::xpath_node_set set = doc.select_nodes(expression.c_str());
		if (set.size() != 0) {
			return true;
		} else {
			return false;
		}
	}

	pugi::xml_node appendValue(pugi::xml_document & doc, string tag, string attribute, string newValue, bool overwriteMultiple = false) {
		if (overwriteMultiple == true) {
			// find the existing node...
			string expression { "//" + tag + "[@" + attribute + "='" + newValue + "']" };
			pugi::xpath_node node = doc.select_node(expression.c_str());
			if (string(node.node().attribute(attribute.c_str()).value()).size() > 0) { // for some reason we get nulls here?
				// ...delete the existing node
				std::cout << "DELETING: " << node.node().name() << ": " << " " << node.node().attribute(attribute.c_str()).value() << std::endl;
				node.node().parent().remove_child(node.node());
			}
		}

		if (!doesTagAndAttributeExist(doc, tag, attribute, newValue)) {
			// otherwise, add it please:
			string expression { "//" + tag + "[@" + attribute + "]" };
			pugi::xpath_node_set add = doc.select_nodes(expression.c_str());
			pugi::xml_node node = add[add.size() - 1].node();
			pugi::xml_node nodeAdded = node.parent().append_copy(node);
			nodeAdded.attribute(attribute.c_str()).set_value(newValue.c_str());
			return nodeAdded;
		} else {
			return pugi::xml_node();
		}
	}

	void addSrc(const fs::path & srcFile, const fs::path & folder) override {
		if (srcFile.empty()) {
			alert("addSrc empty srcFile");
			return;
		}

		string srcFileString = ofPathToString(srcFile); //.string();
		//	fixSlashOrder(srcFileString);
		string folderString = ofPathToString(folder); //.string();
		//	fixSlashOrder(folderString);

		// Made to address ofxGstreamer - adds some core files
		if (folderString == "") {
			folderString = "other";
		}

		// FIXME: Convert to FS::path
		std::vector<string> folderSubNames = ofSplitString(folderString, "\\");
		string folderName = "";
		for (std::size_t i = 0; i < folderSubNames.size(); i++) {
			if (i != 0) folderName += "\\";
			folderName += folderSubNames[i];
			//		alert("appendFilter: " + folderName, 34);
			appendFilter(folderName);
		}

		string ext = srcFile.extension().string();

		if (ext == ".h" || ext == ".hpp" || ext == ".inl") {
			appendValue(projectXml, "ClInclude", "Include", srcFileString);

			pugi::xml_node node = filterXml.select_node("//ItemGroup[ClInclude]").node();
			pugi::xml_node nodeAdded = node.append_child("ClInclude");
			nodeAdded.append_attribute("Include").set_value(srcFileString.c_str());
			nodeAdded.append_child("Filter").append_child(pugi::node_pcdata).set_value(folderString.c_str());

			// appendValue(projectXml, "ClInclude", "Include", srcFileString);

			// pugi::xml_node node = filterXml.select_node("//ItemGroup[ClInclude]").node();
			// pugi::xml_node nodeAdded = node.append_child("ClInclude");
			// nodeAdded.append_attribute("Include").set_value(srcFileString.c_str());
			// nodeAdded.append_child("Filter").append_child(pugi::node_pcdata).set_value(folderString.c_str());
		} else if (ext == ".vert" || ext == ".frag") {
			// TODO: add to None but there's no None in the original template so this fails
			/*appendValue(projectXml, "None", "Include", srcFile);

			pugi::xml_node node = filterXmlDoc.select_node("//ItemGroup[None]").node();
			pugi::xml_node nodeAdded = node.append_child("None");
			nodeAdded.append_attribute("Include").set_value(srcFile.c_str());
			nodeAdded.append_child("Filter").append_child(pugi::node_pcdata).set_value(folder.c_str());*/

		} else if (ext == ".java" || ext == ".kotlin") {
			// Do not add files for other platforms
		} else if (
			ext == ".storyboard" || // Xcode Interface Builder files
			ext == ".xib" || // Xcode Interface Builder files
			ext == ".xcassets" || // Xcode Asset catalogs
			ext == ".xcconfig" || // Xcode build configuration files
			ext == ".entitlements" || // Code signing entitlements (Apple-specific)
			ext == ".plist" || // Property List files (Info.plist, macOS/iOS config)
			ext == ".mm" || // Objective-C++
			ext == ".m" || // Objective-C
			ext == ".swift" || // Swift language files
			ext == ".modulemap" || // Clang module definition (Xcode/Clang-specific)
			ext == ".metal" || // Metal Shading Language (Apple GPU API)
			ext == ".tbd" || // Text-based dynamic libraries (Apple SDKs)
			ext == ".dylib" || // Dynamic libraries (macOS/iOS equivalent of .dll)
			ext == ".framework" || ext == ".bundle" || ext == ".app" || ext == ".xcworkspace" || ext == ".xcodeproj") {
		} else if (
			ext == ".cpp" || ext == ".cxx" || ext == ".cc") {
			appendValue(projectXml, "ClCompile", "Include", srcFileString);

			pugi::xml_node nodeFilters = filterXml.select_node("//ItemGroup[ClCompile]").node();
			pugi::xml_node nodeAdded = nodeFilters.append_child("ClCompile");
			nodeAdded.append_attribute("Include").set_value(srcFileString.c_str());
			nodeAdded.append_child("Filter").append_child(pugi::node_pcdata).set_value(folderString.c_str());
		} else if (ext == ".c") {
			pugi::xml_node node = appendValue(projectXml, "ClCompile", "Include", srcFileString);

			if (!node.child("CompileAs")) {
				pugi::xml_node compileAs = node.append_child("CompileAs");
				compileAs.append_attribute("Condition").set_value("'$(Configuration)|$(Platform)'=='Debug|x64'");
				compileAs.set_value("Default");

				compileAs = node.append_child("CompileAs");
				compileAs.append_attribute("Condition").set_value("'$(Configuration)|$(Platform)'=='Release|x64'");
				compileAs.set_value("Default");
			}

			pugi::xml_node nodeFilters = filterXml.select_node("//ItemGroup[ClCompile]").node();
			pugi::xml_node nodeAdded = nodeFilters.append_child("ClCompile");
			nodeAdded.append_attribute("Include").set_value(srcFileString.c_str());
			nodeAdded.append_child("Filter").append_child(pugi::node_pcdata).set_value(folderString.c_str());
		}

		else {
			alert("File extension ot supported yet on VSProject:" + srcFileString);

			// appendValue(projectXml, "ClCompile", "Include", srcFileString);

			// pugi::xml_node nodeFilters = filterXml.select_node("//ItemGroup[ClCompile]").node();
			// pugi::xml_node nodeAdded = nodeFilters.append_child("ClCompile");
			// nodeAdded.append_attribute("Include").set_value(srcFileString.c_str());
			// nodeAdded.append_child("Filter").append_child(pugi::node_pcdata).set_value(folderString.c_str());
		}
	};
};
