#!/bin/sh
cd "$(dirname "$0")"

ls -lh ./emptyExample.xcodeproj/project.pbxproj
plutil -convert json -r ./emptyExample.xcodeproj/project.pbxproj
ls -lh ./emptyExample.xcodeproj/project.pbxproj

echo "OF XCode template conversion to JSON complete"
