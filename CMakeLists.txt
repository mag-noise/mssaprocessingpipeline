cmake_minimum_required(VERSION 3.15)
# This CMake is reliant on the existence of Eigen3 existing on the system. Use vcpkg manager on windows to also utilize the CMakeSettings.json

option(test "Build all tests." ON)

project(MSSAProcessingPipeline VERSION 0.1.0)
file(GLOB_RECURSE file_search ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.hpp)

add_executable(${PROJECT_NAME} ${file_search})
target_compile_definitions(${PROJECT_NAME} PRIVATE FILE_LOCATION=1)

find_package(Eigen3 REQUIRED NO_MODULE)
# Requires that a version of Matlab be installed and found
find_package(Matlab REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen Matlab::MatlabEngine Matlab::MatlabDataArray)

add_subdirectory(tests)
