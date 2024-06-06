#!/bin/bash

# Check if cmake is installed and install if not
if ! command -v cmake &> /dev/null; then
    echo "cmake could not be found. Installing cmake..."
    sudo apt install cmake -y
fi

# Check if make is installed and install if not
if ! command -v make &> /dev/null; then
    echo "make could not be found. Installing make..."
    sudo apt install make -y
fi

# If ./compile.sh clean
if [ "$1" == "clean" ]; then
    rm -rf build
fi

# Check if build directory exists
if [ ! -d build ]; then
    mkdir build
fi

cd build

# If ./build.sh release enable release mode
if [ "$1" == "release" ]; then
    cmake .. -DCMAKE_BUILD_TYPE=Release
    echo "Release mode enabled"
else
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    echo "Debug mode enabled"
fi

make

# Wait until done compiling and then run tests
if [ $? -eq 0 ]; then
    echo "Compilation successful. Running tests..."
    ctest --output-on-failure 
else
    echo "Compilation failed."
fi