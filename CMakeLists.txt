cmake_minimum_required(VERSION 3.10)

# Set the project name
project(cpplox VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(include)
file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_OSX_SYSROOT "/Library/Developer/CommandLineTools/SDKs/MacOSX.sdk")

# Add the executable
add_executable(cpplox ${SOURCES})