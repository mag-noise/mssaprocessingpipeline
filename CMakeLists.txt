cmake_minimum_required(VERSION 3.15)
# This CMake is reliant on the existence of Eigen3 existing on the system. Use vcpkg manager on windows to also utilize the CMakeSettings.json

project(MSSAProcessingPipeline VERSION 0.1.0)
file(GLOB_RECURSE file_search ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.hpp)

add_executable(${PROJECT_NAME} ${file_search})
find_package(Eigen3 REQUIRED NO_MODULE)

target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)
