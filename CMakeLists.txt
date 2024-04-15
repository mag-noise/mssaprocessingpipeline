
cmake_minimum_required(VERSION 3.18)
# This CMake is reliant on the existence of Eigen3 existing on the system. Use vcpkg manager on windows to also utilize the CMakeSettings.json
option(test "Build all tests." ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mssa)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mssa)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mssa)

message("Bin: ${CMAKE_BINARY_DIR}")

# Setup toolchain and compiler
if(NOT APPLE)
    if(NOT DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED VCPKG_ROOT AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        message(FATAL_ERROR "Variable VCPKG_ROOT must be defined via command line or environment variable, or CMAKE_TOOLCHAIN_FILE must point to vcpkg.cmake.")
    endif()

    if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        if(DEFINED ENV{VCPKG_ROOT})    
            set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
        else()
            set(CMAKE_TOOLCHAIN_FILE ${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
        endif()
    endif()
    message("Toolchain: ${CMAKE_TOOLCHAIN_FILE}")
endif()

set(CMAKE_GENERATOR Ninja)

# MACOS requires install eigen from brew
# Windows requires directly setting C++ compiler via visual studio.
if(APPLE)
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
    set(CMAKE_C_COMPILER "/usr/bin/clang")
elseif(UNIX)
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
    set(CMAKE_C_COMPILER "gcc")
    add_compile_options(-fPIC)
endif()

set(CMAKE_CXX_STANDARD 17)

project(MSSALibraries)

add_subdirectory(MSSAProcessingPipeline)
add_subdirectory(MSSAPython)