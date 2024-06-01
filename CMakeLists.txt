# FILEPATH: /home/vind/auto_edit_project/CMakeLists.txt

# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(auto_edit VERSION 1.0.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the prefix path for finding libraries
set(CMAKE_PREFIX_PATH "/home/vind/doc/vcpkg/installed/x64-linux/lib;/home/vind/doc/vcpkg/installed/x64-linux/share")

# Set the toolchain file for vcpkg
set(CMAKE_TOOLCHAIN_FILE "/home/vind/doc/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Set the root directory for finding ZLIB library
set(ZLIB_ROOT "/home/vind/doc/vcpkg/installed/x64-linux")

# Set the policy for handling if() command behavior
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

# Find the cpr and nlohmann_json packages
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Set the CURL library
set(CURL_LIBRARY "-lcurl") 
find_package(CURL REQUIRED)

# Add the link directories
link_directories(/home/vind/doc/vcpkg/installed/x64-linux/lib)

# Include the source directory
include_directories(${CMAKE_SOURCE_DIR}/include/src)

# Get a list of all source files in the src directory and its subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add main.cpp to the list of source files
list(APPEND SOURCES "main.cpp")

# Add an executable that is built from the source files
add_executable(auto_edit ${SOURCES})

# Link the required libraries to the executable
target_link_libraries(auto_edit PRIVATE cpr nlohmann_json::nlohmann_json CURL::libcurl)