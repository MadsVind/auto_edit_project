# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(auto_edit VERSION 1.0.0)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Enable testing
enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Set the prefix path for finding libraries
set(CMAKE_PREFIX_PATH "${CMAKE_HOME_DIRECTORY}/../doc/vcpkg/installed/x64-linux/lib;${CMAKE_HOME_DIRECTORY}/../doc/vcpkg/installed/x64-linux/share")

# Set the toolchain file for vcpkg
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_HOME_DIRECTORY}/../doc/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Set the root directory for finding ZLIB library
set(ZLIB_ROOT "${CMAKE_HOME_DIRECTORY}/../doc/vcpkg/installed/x64-linux")

set(CURL_LIBRARY "-lcurl") 

# Set the policy for handling if() command behavior
if(POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif()

# Find the cpr and nlohmann_json packages
find_package(cpr CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter) 


# Add the link directories
link_directories("${CMAKE_HOME_DIRECTORY}/../doc/vcpkg/installed/x64-linux/lib")

# Include the directories
include_directories("include")
file(GLOB_RECURSE ALL_INCLUDE_DIRS RELATIVE ${CMAKE_SOURCE_DIR} "src/*.hpp" "src/**/*.hpp" "include/**/*.hpp")
foreach(DIR ${ALL_INCLUDE_DIRS})
    get_filename_component(DIR_PATH ${DIR} PATH)
    include_directories(${DIR_PATH})
endforeach()

# Get a list of all source files in the src directory and its subdirectories
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/**/*.cpp" "include/**/*.cpp")

# Add a library that is built from the source files
add_library(auto_edit_lib ${SOURCES})

# Link the required libraries to the library
target_link_libraries(auto_edit_lib PUBLIC PkgConfig::AVCODEC PkgConfig::AVFORMAT PkgConfig::AVUTIL PkgConfig::AVFILTER cpr nlohmann_json::nlohmann_json CURL::libcurl)

# Add an executable that is built from main.cpp
add_executable(auto_edit main.cpp)

# Link the library to the executable
target_link_libraries(auto_edit PRIVATE auto_edit_lib)

# Add the test subdirectory
add_subdirectory(test)