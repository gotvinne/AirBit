#!/usr/bin/env bash

# A utility script to make building micro:bit projects outside of the micro:bit v2 samples repository
# a little bit easier.
# Author: github.com/c272
# License: GPLv3

# Colour code setup.
RED='\033[0;31m'
CYAN='\033[0;36m'
GREEN='\033[0;32m'
NC='\033[0m'
BOLD=$(tput bold)
NORMAL=$(tput sgr0)

# Print version information.
SCRIPT_DIR=$(dirname "$(realpath "$0")")
COMMIT=$(cd $SCRIPT_DIR && git rev-parse HEAD)
echo -e "${CYAN}microbuild v0.1 (c) C272, 2022${NC}"
echo -e "${CYAN}revision: ${COMMIT:0:10}${NC}\n"

# Set fail-fast errors to on.
set -e

# Run the configuration script.
if [ -f "$SCRIPT_DIR/config.sh" ]; then
    echo "Running configuration script..."
    source "$SCRIPT_DIR/config.sh"
fi

# Darwin-specific setup.
# If we find GNU 'cp', alias 'cp' to it. MacOS 'cp' does not support the -u argument.
if [[ -x "$(command -v gcp)" ]]; then
    echo "Aliasing Darwin binaries to GNU binaries..."
    cp() {
        gcp "$@"
    }
fi

# Parse arguments passed in directly.
for argument in "$@"; do
    key="${argument%%=*}"
    value="${argument#*=}"
    export "$key=$value"
done

# If "MICROBIT_SDK_DIRECTORY" does not exist, error out.
if [[ ! -n "$MICROBIT_SDK_DIRECTORY" ]]; then
  echo -e "${RED}The micro:bit v2 SDK directory was not specified. Either specify this in \`config.sh\`, or via the MICROBIT_SDK_DIRECTORY=... parameter.${NC}"
  exit -1
fi

# If "BUILD_DIRECTORY" does not exist, error out.
if [[ ! -n "$BUILD_DIRECTORY" ]]; then
  echo -e "${RED}The directory to build was not specified. Either specify this in \`config.sh\`, or via the BUILD_DIRECTORY=... parameter.${NC}"
  exit -1
fi

# If "CONTROLLER" does not exist, error out.
if [[ ! -n "$CONTROLLER" ]]; then
  echo -e "${RED}The controller was not specified. Use CONTROLLER=flight or CONTROLLER=hand.${NC}"
  exit -1
fi

# Ensure that the directories exist.
if [[ ! -d "$MICROBIT_SDK_DIRECTORY" ]]; then
  echo -e "${RED}The micro:bit v2 SDK directory provided ('$MICROBIT_SDK_DIRECTORY') does not exist.${NC}"
  exit -1
fi
if [[ ! -d "$BUILD_DIRECTORY" ]]; then
  echo -e "${RED}The build directory provided ('$BUILD_DIRECTORY') does not exist.${NC}"
  exit -1
fi

# Get absolute path of build directory, microbit SDK directory.
MICROBIT_SDK_DIRECTORY=$(realpath "$MICROBIT_SDK_DIRECTORY")
BUILD_DIRECTORY=$(realpath "$BUILD_DIRECTORY")

# Ensure that the micro:bit SDK directory provided is *actually* an SDK directory.
if [[ ! -f "$MICROBIT_SDK_DIRECTORY/build.py" ]]; then
  echo -e "${RED}No 'build.py' file was found in provided SDK directory ('$MICROBIT_SDK_DIRECTORY'). Are you sure this is a valid micro:bit SDK directory?${NC}"
  exit -1
fi

# Select the base configuration.
BASE_CODAL_JSON="$SCRIPT_DIR/microbuild/codal.json"
SDK_CODAL_JSON="$MICROBIT_SDK_DIRECTORY/codal.json"

if [[ -f "$BUILD_DIRECTORY/codal.json" ]]; then
    BASE_CODAL_JSON="$BUILD_DIRECTORY/codal.json"
fi

# Write the configuration for the selected controller.
CONTROLLER_CHANGED=$(python3 "$SCRIPT_DIR/configure_controller.py" \
    --controller "$CONTROLLER" \
    --codalpath "$BASE_CODAL_JSON")

# Keep an existing correct symlink.
if [[ -L "$SDK_CODAL_JSON" ]] &&
   [[ "$(readlink "$SDK_CODAL_JSON")" == "$BASE_CODAL_JSON" ]]; then
    echo "Reusing existing codal.json symlink."
else
    # Remove an old link or regular SDK configuration.
    if [[ -L "$SDK_CODAL_JSON" || -f "$SDK_CODAL_JSON" ]]; then
        rm "$SDK_CODAL_JSON"
    fi

    ln -s "$BASE_CODAL_JSON" "$SDK_CODAL_JSON"
fi

if [[ "$CONTROLLER_CHANGED" == "true" ]]; then
    echo -e "${CYAN}Controller changed. Cleaning build directory...${NC}"
    rm -rf -- "$MICROBIT_SDK_DIRECTORY/build"
fi

# Clear existing library symlinks.
find "$MICROBIT_SDK_DIRECTORY/libraries" -type l -delete

# If there is a ".microbuild" file in the build directory, add each of the given subdirectories
# as library directories.
if [[ -f "$BUILD_DIRECTORY/.microbuild" ]]; then
    echo -e "${CYAN}Detected '.microbit' file, adding specified includes to CODAL environment...${NC}"
    ORIGINAL_PWD="$PWD"
    cd "$BUILD_DIRECTORY"
  
    # Imports are relative to the build directory, so we moved there first.
	while IFS= read -r LINE || [[ -n "$LINE" ]]; do
		ln -s $(realpath "$BUILD_DIRECTORY/$LINE") "$MICROBIT_SDK_DIRECTORY/libraries/"
	done < "$BUILD_DIRECTORY/.microbuild"
    cd "$ORIGINAL_PWD"
fi

# Begin the build.
echo -e "${CYAN}Beginning build...${NC}"
ORIGINAL_PWD="$PWD"
cd "$MICROBIT_SDK_DIRECTORY"
python3 build.py $BUILD_ARGS
cd "$ORIGINAL_PWD"

# Copy result of build to output folder, if specified.
if [[ -n "$BUILD_OUTPUT_DIRECTORY" ]]; then

    echo -e "${CYAN}Copying build output to output directory...${NC}"

    # Ensure it exists.
    BUILD_OUTPUT_DIRECTORY=$(realpath "$BUILD_OUTPUT_DIRECTORY")
    mkdir -p "$BUILD_OUTPUT_DIRECTORY"

    # Copy the micro:bit hex file over.
    cp -u "$MICROBIT_SDK_DIRECTORY/MICROBIT.hex" "$BUILD_OUTPUT_DIRECTORY"
fi

echo -e "${GREEN}${BOLD}Build complete!${NORMAL}${NC}"