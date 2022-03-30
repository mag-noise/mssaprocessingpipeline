# CMake files for project GTest framework
cmake_minimum_required(VERSION 3.15)


project(ProcessingTests VERSION 0.1.0)
get_filename_component(PROJECT_PARENT ${PROJECT_SOURCE_DIR} DIRECTORY)
file(GLOB_RECURSE file_search ${PROJECT_PARENT}/src/*.cpp ${PROJECT_PARENT}/src/*.hpp)
list(REMOVE_ITEM file_search ${PROJECT_PARENT}/src/main.cpp)

enable_testing()
add_executable(${PROJECT_NAME} ${file_search} ${PROJECT_SOURCE_DIR}/Test.cpp ${PROJECT_SOURCE_DIR}/TestPreProcessor.cpp ${PROJECT_SOURCE_DIR}/TestProcessor.cpp)

find_package(GTest REQUIRED)
find_package(Eigen3 REQUIRED NO_MODULE)

target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})