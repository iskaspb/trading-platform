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
include_directories(${CMAKE_SOURCE_DIR}/core)

# boost
set(Boost_DEBUG FALSE)
set(Boost_USE_STATIC_LIBS ON CACHE INTERNAL "" FORCE)
set(Boost_USE_RELEASE_LIBS ON CACHE INTERNAL "" FORCE)
find_package(Boost COMPONENTS filesystem iostreams program_options regex log SYSTEM REQUIRED)
message(STATUS "Found Boost ${Boost_VERSION}")
message(STATUS "Found Boost ${Boost_INCLUDE_DIRS}")

# nlohmann json
find_package(nlohmann_json 3.11 REQUIRED)

# rapid json
find_package(RapidJSON)


add_subdirectory(apps)
add_subdirectory(tests)
