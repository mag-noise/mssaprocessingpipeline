
cmake_minimum_required(VERSION 3.27)
# This CMake is reliant on the existence of Eigen3 existing on the system. Use vcpkg manager on windows to also utilize the CMakeSettings.json
option(test "Build all tests." ON)
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/mssa)
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

if(NOT DEFINED ENV{CMAKE_CUDA_COMPILER} AND NOT DEFINED CMAKE_CUDA_COMPILER)
    message("No Cuda")
    project(MSSAProcessingPipeline VERSION 1.2.1 LANGUAGES CXX)
else()
    message("Has Cuda")
    project(MSSAProcessingPipeline VERSION 1.2.1 LANGUAGES CXX CUDA)
endif()
message("${CMAKE_BUILD_TYPE}")

file(GLOB_RECURSE file_search ${PROJECT_SOURCE_DIR}/MSSAProcessingPipeline/src/*.cpp ${PROJECT_SOURCE_DIR}/MSSAProcessingPipeline/src/*.hpp)

list(REMOVE_ITEM file_search ${PROJECT_SOURCE_DIR}/MSSAProcessingPipeline/src/mexEntry.cpp)
list(REMOVE_ITEM file_search ${PROJECT_SOURCE_DIR}/MSSAProcessingPipeline/src/pyEntry.cpp)


message("Prefix: ${CMAKE_PREFIX_PATH}")
if(DEFINED ENV{EIGEN3_INCLUDE_DIR})
    include_directories("$ENV{EIGEN3_INCLUDE_DIR}")
    set(EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}")
else()
    find_package(Eigen3 REQUIRED)
endif()

# Probably redundant, but at least tests that the check works
if(NOT DEFINED ENV{CMAKE_CUDA_COMPILER} AND NOT DEFINED CMAKE_CUDA_COMPILER )
    set(NO_ML ON)
else()
    set(NO_ML OFF)
endif(NO_ML)

find_package(Torch CONFIG)
if(Torch_FOUND)
    include_directories(SYSTEM ${TORCH_INCLUDE_DIRS})
else()
    list(REMOVE_ITEM file_search ${PROJECT_SOURCE_DIR}/MSSAProcessingPipeline/src/Utilities/ModelInjector.hpp)
endif(Torch_FOUND)

# Generate Library based on what's allowed by packages found
add_library(${PROJECT_NAME} STATIC ${file_search})
add_library(sub::mssaLib ALIAS ${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME} PRIVATE FILE_LOCATION=1)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

#include_directories(${EIGEN_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

 # this is heuristically generated, and may not be correct
find_package(Caffe2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotobuf)

message(${Torch_FOUND})

if(Torch_FOUND AND NOT ${NO_ML})
    message("Including PyTorch libraries...")
    find_package(mimalloc REQUIRED)
	add_compile_definitions(_TORCH)
	target_link_libraries(${PROJECT_NAME} PRIVATE mimalloc)
    target_link_libraries(${PROJECT_NAME} PRIVATE "${TORCH_LIBRARIES}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
    #target_link_libraries(${PROJECT_NAME}  "${TORCH_LIBRARIES}")

    # The following code block is suggested to be used on Windows.
    # According to https://github.com/pytorch/pytorch/issues/25457,
    # the DLLs need to be copied to avoid memory errors.
    if (MSVC)
        file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")

        # do the copying
        foreach( file_i ${TORCH_DLLS})
            add_custom_command(
            TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy ${file_i} ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        )
        endforeach( file_i )
    endif (MSVC)
endif(Torch_FOUND)
# Requires that a version of Matlab be installed and found. If not found, add to PATH or Matlab_ROOT_DIR
find_package(Matlab)
if(Matlab_FOUND)
	target_link_libraries(${PROJECT_NAME} PRIVATE Matlab::MatlabEngine Matlab::MatlabDataArray)
    matlab_add_mex(NAME MSSAMex SHARED SRC ${PROJECT_SOURCE_DIR}/MSSAProcessingPipeline/src/mexEntry.cpp LINK_TO ${PROJECT_NAME} Eigen3::Eigen Matlab::mex Matlab::mx DOCUMENTATION ${PROJECT_SOURCE_DIR}/MSSAProcessingPipeline/docs/MSSAMex.txt)    
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message("Debug Mode")	
    enable_testing()
    add_subdirectory(MSSAProcessingPipeline/tests)
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    message("Release Mode MSSALibraries")

    add_custom_command(TARGET MSSAMex
                    POST_BUILD
                    COMMAND ${CMAKE_COMMAND} 
                    -DCMAKE_RELEASE_LOCATION=${PROJECT_SOURCE_DIR}/release 
                    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                    -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                    -DEIGEN3_INCLUDE_DIRS=${EIGEN3_INCLUDE_DIRS}/Eigen
                    -P ${PROJECT_SOURCE_DIR}/zipfiles.cmake
    )
endif()
configure_file(${PROJECT_SOURCE_DIR}/MSSAPython/pyproject.toml ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/README.md ${CMAKE_LIBRARY_OUTPUT_DIRECTORY} COPYONLY)

add_subdirectory(MSSAPython)
