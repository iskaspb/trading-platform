cmake_minimum_required(VERSION 3.20)
project(trading_platform)

# Ensure CMake is run in the build directory
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "CMake must be run in a separate build directory!")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define compile flags for debug/release build modes
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
add_compile_options(-Wall -Wextra -Wpedantic)

# Put all binaries in the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable testing using ctest
enable_testing()

# Add include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/core)

# Current Boost setup depends on auto-linking feature of Boost, which means you don't need to define anything except ${Boost_LIBRARIES} in dependencies
set(Boost_DEBUG FALSE)
#set(Boost_USE_STATIC_LIBS ON CACHE INTERNAL "" FORCE) # Review more efficient boost linking when start perf optimization
set(Boost_USE_RELEASE_LIBS ON CACHE INTERNAL "" FORCE)
find_package(Boost REQUIRED COMPONENTS filesystem iostreams program_options regex log_setup log)
message(STATUS "Found Boost ${Boost_VERSION}, includes: ${Boost_INCLUDE_DIRS}, libs: ${Boost_LIBRARIES}")

# nlohmann json
find_package(nlohmann_json 3.11 REQUIRED)

# rapid json
find_package(RapidJSON)

# metal MPL library is header only, so no need to link anything
include_directories(${CMAKE_SOURCE_DIR}/external/metal/include/)

add_subdirectory(apps)
add_subdirectory(tests)
