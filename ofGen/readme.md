# ofGen

# generate templates
ofbuild gen
# configure
ofbuild

current folder: cwd
I can't find an OF installation in default path ../../..
edit ofpath? (y)es, (n)o, (q)uit


ofGen -platforms=zed,macos -addons=ofxMicroUI,ofxTools -ofroot=


../../../apps/pgd/commandLine/bin/projectGenerator
-o"../../.."
-p"macos,zed"
-a"ofxTools,ofxMicroUI,ofxScenes,ofxAssimp,ofxNetwork,ofxTweeny,ofxMidi" .

meant to work as projectGenerator

## Pseudocode
check if ofPath is valid. if it is not, show help and exit
Parse addons on parameters if they exist
if not, look for addons.make, and parse them, if they exist.
list addons in project folder
iterate them in local or addons folder, load to memory (array of addons) storing final path.
this will serve to investigate when the same name addon is used locally and addons folder.
parse all addons for more addons dependencies, add to the list if it is missing.

create project(s) in memory or more, depending on the number of platforms/templates.




if we use more projects, do the same for each of them.


## Thoughts
- we now have platforms (operational systems?) (windows, linux)
- we have targets (ios, macos, android etc.)
- we have IDEs tied to platforms (xcode, visual studio)
- and IDEs attending multiple platforms/targets (VSCode, ZED)
- and build systems (Make) attending multiple platforms/targets
