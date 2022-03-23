cmake_minimum_required(VERSION 3.15)


project(MSSAProcessingPipelineLibrary VERSION 0.0.1)
get_filename_component(PROJECT_PARENT ${PROJECT_SOURCE_DIR} DIRECTORY)
file(GLOB_RECURSE file_search ${PROJECT_PARENT}/src/*.cpp ${PROJECT_PARENT}/src/*.hpp)
list(REMOVE_ITEM file_search ${PROJECT_PARENT}/src/main.cpp)

add_library(${PROJECT_NAME} ${file_search})
find_package(Eigen3 REQUIRED NO_MODULE)

target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)
