cmake_minimum_required(VERSION 3.15)
# This CMake is reliant on the existence of Eigen3 existing on the system. Use vcpkg manager on windows to also utilize the CMakeSettings.json
option(test "Build all tests." ON)

project(MSSAProcessingPipeline VERSION 0.1.0)
file(GLOB_RECURSE file_search ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.hpp)
list(REMOVE_ITEM file_search ${PROJECT_SOURCE_DIR}/src/mexEntry.cpp)


add_library(${PROJECT_NAME} STATIC ${file_search})
target_compile_definitions(${PROJECT_NAME} PRIVATE FILE_LOCATION=1)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
find_package(Eigen3 REQUIRED NO_MODULE)


include_directories(${EIGEN_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

# Requires that a version of Matlab be installed and found. If not found, add to PATH or Matlab_ROOT_DIR
find_package(Matlab REQUIRED)
if(Matlab_FOUND)
	target_link_libraries(${PROJECT_NAME} PRIVATE Matlab::MatlabEngine Matlab::MatlabDataArray)
	matlab_add_mex(NAME mssa EXECUTABLE SRC ${PROJECT_SOURCE_DIR}/src/mexEntry.cpp LINK_TO ${PROJECT_NAME} Eigen3::Eigen Matlab::mex Matlab::mx Matlab::MatlabEngine Matlab::MatlabDataArray)
endif()
if(CMAKE_BUILD_TYPE EQUAL "DEBUG")
message("Debug Mode")	
add_subdirectory(tests)
else()
message("Release Mode")
endif()
