cmake_minimum_required(VERSION 3.20)
project(trading_platform)

# Ensure CMake is run in the build directory
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMake must be run in a separate build directory!")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define build modes
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Put all binaries in the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable testing using ctest
enable_testing()

# Add include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add subdirectories for each binary
add_subdirectory(tests)
