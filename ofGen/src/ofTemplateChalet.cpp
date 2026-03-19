#include "ofTemplateChalet.h"
#include "addons.h"
#include "genConfig.h"
#include <fmt/chrono.h> // fmt’s chrono integration
#include <fstream>

#include <vector>       // std::vector
// #include <cstdint>      // uint8_t, int32_t
// #include <algorithm>    // std::min
// #include <filesystem>   // std::filesystem (C++17)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  // (already included in your project presumably)

ofTemplateChalet::ofTemplateChalet() {
	name = "chalet";
	path = conf.getTemplatesFolder() / name;

	// openCommand = "chalet . ";
	commands["build"] = "chalet build";
	commands["run"] = "chalet buildrun";
	commands["clean"] = "chalet clean --all";
	commands["bundle"] = "chalet bundle; cp -r dist/* bin";

	// buildCommand = "chalet build";
	// runCommand = "chalet buildrun";
	// cleanCommand = "chalet clean --all";
	// bundleCommand = "chalet bundle; cp dist/* bin";
}

std::string timeString() {
	return fmt::format("{:%Y%m%d-%H%M}", std::chrono::system_clock::now()); // GMT / UTC
}

void ofTemplateChalet::load() {
	alert("ofTemplateChalet::load()", 92);

	fs::path projectFrom { path / "chalet.yaml" };

	std::vector<std::string> addonsNames;
	for (auto & a : conf.addons) {
		addonsNames.emplace_back(a->name);
	}

	projectYaml = YAML::LoadFile(projectFrom.string());

	projectYaml["name"] = conf.projectName;

	if (conf.settings.contains("version")) {
		projectYaml["version"] = conf.settings["version"];
	}

	for (const auto & i : conf.infoPlist) {
		alert("	▸ infoPlist " + i.first + ": " + i.second, 90);
		projectYaml["distribution"]["empty"]["macosBundle"]["infoPropertyList"][i.first] = i.second;

		// YAML::Node targetNode = projectYaml["distribution"]["empty"]["macosBundle"]["infoPropertyList"][i.first];
		// targetNode = i.second;
		// targetNode.SetTag("!!str");
	}

	projectYaml["variables"]["platform"] = getPlatformString();
	projectYaml["variables"]["addons"] = joinStrings(addonsNames, ",");
	projectYaml["variables"]["generator"] = getVersion();
	projectYaml["variables"]["time"] = timeString();

	if (fs::exists("icon") && fs::is_directory("icon")) {
		bool hasIco = false;
		bool hasPng = false;
		fs::path pngPath;
		for (auto const & i : fs::directory_iterator { "icon" }) {
			std::string found;
			if (i.path().extension() == ".ico") {
				alert("	▸ icon found " + i.path().string(), 94);
				projectYaml["targets"]["empty"]["settings:Cxx"]["windowsApplicationIcon"] = i.path().string();
				hasIco = true;
			}
			if (i.path().extension() == ".icns") {
				alert("	▸ icon found " + i.path().string(), 94);
				projectYaml["distribution"]["empty"]["macosBundle"]["icon"] = i.path().string();
			}

			if (i.path().extension() == ".png") {
				pngPath = i.path();
				alert("	▸ icon found " + i.path().string(), 94);
				projectYaml["distribution"]["empty"]["linuxDesktopEntry"]["icon"] = i.path().string();
				projectYaml["distribution"]["empty"]["linuxDesktopEntry"]["template"] = "platform/linux/app.desktop";
				projectYaml["distribution"]["empty"]["macosBundle"]["icon"] = i.path().string();
				hasPng = true;
			}
		}

		alert ("getPlatformString() " + getPlatformString(), 95);

		if (getPlatformString() == "vs" && !hasIco && hasPng) {
			fs::path icoPath { fs::path { "icon" } / (conf.projectName + ".ico") };

			int w, h, ch;
			unsigned char * src = stbi_load(pngPath.string().c_str(), &w, &h, &ch, 4);
			if (!src) {
				alert("  ! failed to load " + pngPath.string(), 31);
				// continue;
			}

			// Target standard Windows icon sizes (0 in ICO means 256)
			const std::vector<int> targets = { 16, 32, 48, 64, 128, 256 };
			struct Image {
				int size;
				std::vector<uint8_t> bgra;
			};
			std::vector<Image> images;

			// Crop to center square if non-square, then resize
			int cropSize = std::min(w, h);
			int offX = (w - cropSize) / 2;
			int offY = (h - cropSize) / 2;

			for (int s : targets) {
				if (cropSize < s) continue; // Skip if source too small

				Image img;
				img.size = s;
				img.bgra.resize(s * s * 4);

				float scale = float(cropSize) / float(s);
				for (int y = 0; y < s; ++y) {
					for (int x = 0; x < s; ++x) {
						int sx = offX + int(x * scale);
						int sy = offY + int(y * scale);
						uint8_t * p = src + (sy * w + sx) * 4;
						uint8_t * d = &img.bgra[(y * s + x) * 4];
						d[0] = p[2]; // B
						d[1] = p[1]; // G
						d[2] = p[0]; // R
						d[3] = p[3]; // A
					}
				}
				images.push_back(std::move(img));
			}

			// Also add original if it's square and < 256 (optional)
			if (w == h && w < 256 && w > 16) {
				Image img;
				img.size = w;
				img.bgra.resize(w * h * 4);
				for (int i = 0; i < w * h; ++i) {
					img.bgra[i * 4 + 0] = src[i * 4 + 2];
					img.bgra[i * 4 + 1] = src[i * 4 + 1];
					img.bgra[i * 4 + 2] = src[i * 4 + 0];
					img.bgra[i * 4 + 3] = src[i * 4 + 3];
				}
				images.push_back(std::move(img));
			}

			if (!images.empty()) {
				std::ofstream file(icoPath, std::ios::binary);
				if (file.is_open()) {
					// ICONDIR (6 bytes)
					file.put(0);
					file.put(0); // Reserved
					file.put(1);
					file.put(0); // Type: Icon
					file.put(images.size() & 0xFF);
					file.put(0); // Count (low byte only, assuming < 256)

					// Calculate offsets
					int headerSize = 6 + images.size() * 16;
					std::vector<int> offsets;
					std::vector<int> sizes;
					int offset = headerSize;

					for (auto & img : images) {
						int andMaskRow = ((img.size + 31) / 32) * 4;
						int imgSize = 40 + img.size * img.size * 4 + andMaskRow * img.size; // BITMAPINFOHEADER + XOR + AND
						offsets.push_back(offset);
						sizes.push_back(imgSize);
						offset += imgSize;
					}

					// ICONDIRENTRIES (16 bytes each)
					for (size_t i = 0; i < images.size(); ++i) {
						int s = images[i].size;
						file.put(s == 256 ? 0 : (uint8_t)s); // Width (0 = 256)
						file.put(s == 256 ? 0 : (uint8_t)s); // Height
						file.put(0); // Colors (0 = > 256)
						file.put(0); // Reserved
						file.put(1);
						file.put(0); // Planes
						file.put(32);
						file.put(0); // BitCount
						// Size
						file.put((sizes[i] >> 0) & 0xFF);
						file.put((sizes[i] >> 8) & 0xFF);
						file.put((sizes[i] >> 16) & 0xFF);
						file.put((sizes[i] >> 24) & 0xFF);
						// Offset
						file.put((offsets[i] >> 0) & 0xFF);
						file.put((offsets[i] >> 8) & 0xFF);
						file.put((offsets[i] >> 16) & 0xFF);
						file.put((offsets[i] >> 24) & 0xFF);
					}

					// Write image data (BITMAPINFOHEADER + XOR mask + AND mask)
					for (auto & img : images) {
						int s = img.size;
						int32_t hdr[10] = {
							40, s, s * 2, 1 | (32 << 16), 0, 0, 0, 0, 0, 0
						};
						file.write((char *)hdr, 40);

						// XOR mask (BGRA, bottom-up)
						for (int y = s - 1; y >= 0; --y) {
							file.write((char *)&img.bgra[y * s * 4], s * 4);
						}

						// AND mask (1bpp, bottom-up, padded to 4 bytes, all 0 for 32-bit alpha)
						int andRow = ((s + 31) / 32) * 4;
						std::vector<uint8_t> zeros(andRow, 0);
						for (int y = 0; y < s; ++y) {
							file.write((char *)zeros.data(), andRow);
						}
					}

					file.close();
					projectYaml["targets"]["empty"]["settings:Cxx"]["windowsApplicationIcon"] = icoPath.string();
					alert("  ▸ generated " + icoPath.string() + " (" + std::to_string(images.size()) + " sizes)", 92);
				}
			}
			stbi_image_free(src);
			projectYaml["targets"]["empty"]["settings:Cxx"]["windowsApplicationIcon"] = icoPath.string();
		}
	}

	// MARK: Not ok to remove since addonToChalet only include defines from ADDON_DEFINES, not from of.yml define
	for (auto & d : conf.defines) {
		alert("	Add define chalet " + d);
		projectYaml["targets"]["empty"]["settings:Cxx"]["defines"].push_back(d);
	}

	// for (auto & f : conf.frameworks) {
	// 	projectYaml["abstracts:*"]["appleFrameworks"].push_back(f);
	// }

	// for (auto & a : conf.addons) {
	// for (auto & l : a->filteredMap["libs"]) {
	// 	string libPath { "${var:ofPath}/addons/" + a->name + '/' + l.string() };
	// 	projectYaml["targets"]["empty"]["settings:Cxx"]["staticLinks"].push_back(libPath);
	// }
	// for (auto & f : a->filteredMap["includes"]) {
	// }
	// }
}

void ofTemplateChalet::addAddon(ofAddon * a) {
	// alert(" ofTemplateChalet::addAddon() " + a->name, 91);

	std::string folder { a->isLocal ? a->path.string() + "/" : "${var:ofPath}/addons/" + a->name + "/" };
	if (a->isProject) {
		folder = "";
	}
	// alert ("FOLDER = " + folder, 95);
	// alert ("a->path = " + a->path.string(), 95);

	for (auto & f : a->filteredMap["sources"]) {
		std::string path { folder + f.generic_string() };
		projectYaml["targets"]["empty"]["files"]["include"].push_back(path);
	}
	// for (auto & f : a->filteredMap["headers"]) {
	// }

	for (auto & f : a->filteredMap["includes"]) {
		std::string path { folder + f.string() };
		// projectYaml["abstracts:*"]["settings:Cxx"]["includeDirs"].push_back(path);
		projectYaml["targets"]["empty"]["settings:Cxx"]["includeDirs"].push_back(path);
	}

	for (auto & f : a->filteredMap["libs"]) {
		// fs::path p { a->path / f };
		// std::string path { "${var:ofPath}/addons/" + a->name + "/" + f.string() };

		std::string path { folder + f.string() };
		projectYaml["targets"]["empty"]["settings:Cxx"]["staticLinks"].push_back(path);
	}

	if (a->addonProperties.count("ADDON_FRAMEWORKS")) {
		for (const auto & f : a->addonProperties["ADDON_FRAMEWORKS"]) {
			for (auto & s : ofSplitString(f, " ")) {
				// alert("     appleFramework " + s, 95);
				alert("	▸ appleFramework " + s, 95);
				// projectYaml["abstracts:*"]["settings:Cxx"]["appleFrameworks"].push_back(s);
				size_t found = s.find('/');
				if (found != std::string::npos) {
					alert("	▸ appleFramework inside Addon " + s, 94);
					projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworks"].push_back(fs::path(s).stem().string());
					std::string frameworkPath = std::string("${var:ofPath}/addons/") + a->name + "/" + fs::path(s).parent_path().string();
					alert("frameworkPath :: " + frameworkPath, 92);
					projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworkPaths"].push_back(frameworkPath);
				} else {
					projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworks"].push_back(s);
				}
			}
		}
	}

	// FIXME: TODO: handle cflags etc.
	const std::map<std::string, std::string> addonToChalet {
		{ "ADDON_CFLAGS", "compileOptions" },
		{ "ADDON_CPPFLAGS", "compileOptions" },
		{ "ADDON_LDFLAGS", "linkerOptions" },
		{ "ADDON_DEFINES", "defines" },
	};

	for (auto & param : addonToChalet) {
		if (a->addonProperties.count(param.first)) {
			for (const auto & property : a->addonProperties[param.first]) {
				for (const auto & s : ofSplitString(property, " ")) {
					alert("	└─ " + param.second + " : " + s, 94);
					projectYaml["targets"]["empty"]["settings:Cxx"][param.second].push_back(s);
					// if (!projectYaml["targets"]["empty"]["settings:Cxx"][param.second][s]) {
					// 	projectYaml["targets"]["empty"]["settings:Cxx"][param.second].push_back(s);
					// }
				}
			}
		}
	}

	for (const fs::path & f : a->filteredMap["frameworks"]) {
		// addFramework(a->path / f);
		alert("	▸ appleFramework " + f.string(), 91);
		// projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworks"].push_back(f.string());

		std::string s = f.string();
		size_t found = s.find('/');
		if (found != std::string::npos) {
			alert("	└─ appleFramework inside Addon " + s, 91);
			projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworks"].push_back(fs::path(s).stem().string());
			std::string frameworkPath = std::string("${var:ofPath}/addons/") + a->name + "/" + fs::path(s).parent_path().string();
			alert("	frameworkPath :: " + frameworkPath, 92);
			projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworkPaths"].push_back(frameworkPath);
		} else {
			projectYaml["targets"]["empty"]["settings:Cxx"]["appleFrameworks"].push_back(s);
		}
	}

	// I'm now removing this one. it was handled already by conf.defines in general addon loading. ofAddon::loadFiles populating conf.defines
	// for (auto & p : a->addonProperties["ADDON_DEFINES"]) {
	// 	projectYaml["targets"]["empty"]["settings:Cxx"]["defines"].push_back(p);
	// }
}

void ofTemplateChalet::save() {

	renameYamlKey(projectYaml["targets"], "empty", conf.projectName);
	projectYaml["distribution"]["empty"]["buildTargets"] = conf.projectName;
	renameYamlKey(projectYaml["distribution"], "empty", conf.projectName);

	renameYamlKey(projectYaml["distribution"]["empty.dmg"]["positions"], "empty", conf.projectName);
	renameYamlKey(projectYaml["distribution"], "empty.dmg", conf.projectName + ".dmg");

	// Change key "empty" to project name in targets
	// {
	// 	auto targets = projectYaml["targets"];
	// 	YAML::Node emptyNode = targets["empty"];
	// 	targets[conf.projectName] = emptyNode;
	// 	targets.remove("empty");
	// }

	// // Change key "empty" to project name in distribution
	// {
	// 	auto distribution = projectYaml["distribution"];
	// 	YAML::Node emptyNode = distribution["empty"];
	// 	distribution[conf.projectName] = emptyNode;
	// 	distribution.remove("empty");
	// }

	alert("ofTemplateChalet::save()", 92);

	fs::path projectTo { conf.projectPath / "chalet.yaml" };
	// alert(projectTo.string(), 35);
	std::ofstream saveFile(projectTo.string());
	// std::string comment = "generator " + getVersion();
	// saveFile << YAML::Comment(comment) << projectYaml;
	saveFile << projectYaml;
	saveFile.close();
}
