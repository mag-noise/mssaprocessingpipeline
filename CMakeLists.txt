cmake_minimum_required(VERSION 3.15)

project(MSSAProcessingPipeline VERSION 0.0.1)
add_executable(${PROJECT_NAME} src/main.cpp)
#add_library(MSSAProcessor src/MSSA/MSSA.cpp src/MSSA/MSSA.h)

#add_subdirectory(external/Eigen)
find_package(Eigen3 REQUIRED NO_MODULE)
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)