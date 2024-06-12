#!/bin/bash

# Install the necessary libraries if they are not already installed
if ! dpkg -l | grep -q libmlt-dev; then
    sudo apt-get update
    sudo apt-get install -y libmlt-dev
fi

if ! dpkg -l | grep -q libcurl4-openssl-dev; then
    sudo apt-get update
    sudo apt-get install -y libcurl4-openssl-dev
fi

if ! dpkg -l | grep -q libnlohmann-json-dev; then
    sudo apt-get update
    sudo apt-get install -y libnlohmann-json-dev
fi

# Install cpr
if ! dpkg -l | grep -q libcpr-dev; then
    sudo apt-get update
    sudo apt-get install -y libcpr-dev
fi

# Set the MLT_REPOSITORY environment variable
export MLT_REPOSITORY=/usr/lib/x86_64-linux-gnu/mlt-7/

# Create a build directory and navigate into it
mkdir -p build
cd build

# Run CMake to generate the Makefile
cmake ..

# Run make to build the project
make

# Run the tests
#ctest

# If the tests were successful, run the program
if [ $? -eq 0 ]; then
    ./auto_edit
fi