cmake_minimum_required(VERSION 3.15)


project(MSSAProcessingPipeline VERSION 0.0.1)
add_executable(${PROJECT_NAME} src/main.cpp)
find_package(Eigen3 REQUIRED NO_MODULE)

add_subdirectory(src/MSSA)
target_link_libraries(${PROJECT_NAME} PRIVATE MSSAProcessor Eigen3::Eigen)