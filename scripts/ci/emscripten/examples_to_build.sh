#!/bin/bash

# List of examples to build - add emscripten compatible examples to this list
# NOTE: make sure there is no trailing slash!!!
folders=(
#3d
    "examples/3d/pointCloudExample"
    "examples/3d/3DPrimitivesExample"
    "examples/3d/ofxAssimpBoneControlExample"
#   "examples/3d/ofxAssimpAdvancedExample" #broken currently
    "examples/3d/ofNodeExample"
    "examples/3d/multiTexture3dExample"
    "examples/3d/modelNoiseExample"
#gl
    "examples/gl/shadowsExample"
    "examples/gl/materialPBR"
    "examples/gl/materialPBRAdvanced"
    "examples/gl/vboMeshDrawInstancedExample"
#math
    "examples/math/noise1dOctaveExample"
    "examples/math/particlesExample"
    "examples/math/periodicSignalsExample"
    "examples/math/trigonometryExample"
    "examples/math/trigonometricMotionExample"
    
    # Add more paths as needed
)

cur_root=$(pwd);
script_path="$(cd "$(dirname "$0")" && pwd)"

cd $cur_root;
mkdir -p out
out_folder="$cur_root/out"

outPaths=""
outThumbs=""

# Iterate through the folder paths
for folder in "${folders[@]}"; do
    # Check if the folder exists
    if [ -d "$folder" ]; then
    
		parent_folder=$(dirname "$folder")
		parent_folder_name=$(basename "$parent_folder")

        # Change to the directory
        cd $folder
        cp ../../../scripts/templates/emscripten/Makefile .
		cp ../../../scripts/templates/emscripten/config.make .
        emmake make -j2 Release
        
        errorcode=$?
		if [[ $errorcode -ne 0 ]]; then
			echo "Couldn't build emscripten example: $folder"
		else
			folder_name=$(basename "$folder")
			mkdir -p "$out_folder/$parent_folder_name"
			cp -r "bin/em/$folder_name" "$out_folder/$parent_folder_name/"
			
			thumb_png="$folder_name.png"
			thumb_gif="$folder_name.gif"
			thumb_jpg="$folder_name.jpg"
			
			if [ -e "$thumb_png" ]; then
				cp -r $thumb_png "$out_folder/$parent_folder_name/$folder_name/"
				outThumbs+="$thumb_png,"
			elif [ -e "$thumb_gif" ]; then
				cp -r $thumb_gif "$out_folder/$parent_folder_name/$folder_name/"
				outThumbs+="$thumb_gif,"
			elif [ -e "$thumb_jpg" ]; then
				cp -r $thumb_jpg "$out_folder/$parent_folder_name/$folder_name/"
				outThumbs+="$thumb_jpg,"
			else
				outThumbs+="of.png,"
			fi
			
			outPaths+="$parent_folder_name/$folder_name,"
		fi
		
		cd $cur_root
    else
        echo "Folder does not exist: $folder"
    fi
done

cd $cur_root;

# Remove the trailing comma
outPaths=${outPaths%,}
outThumbs=${outThumbs%,}

htmlFile="$out_folder/index.html"

echo "outPaths is $outPaths"
echo "html is $htmlFile"

# Replace the placeholder in the template file
cp -r $script_path/index.html $htmlFile
sed -i "s|REPLACE_ME|$outPaths|g" $htmlFile
sed -i "s|REPLACE_FILES|$outThumbs|g" $htmlFile

DO_UPLOAD="false"

if [[ "$GH_ACTIONS" = true && "${GH_BRANCH}" == "master" && -z "${GH_HEAD_REF}" ]]; then
	echo "upload 1/2 - make key file"
    # Temporary file to store the private key
	key_file=$(mktemp)
	echo -e "$GA_EXAMPLES_KEY" > "$key_file"
	chmod 600 "$key_file"
    DO_UPLOAD="true";
fi

if [ "$DO_UPLOAD" = "true" ]; then
	echo "upload 2/2 - time for rsync"
	remote_path="/home/$GA_EXAMPLES_USER/examples/"
	rsync -avz -e "ssh -o 'StrictHostKeyChecking no' -i $key_file" "$out_folder/" "$GA_EXAMPLES_USER@$GA_EXAMPLES_SERVER:$remote_path"
    rm -f "$key_file"
fi
