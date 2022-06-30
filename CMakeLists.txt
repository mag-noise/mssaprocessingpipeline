cmake_minimum_required(VERSION 3.18)
# This CMake is reliant on the existence of Eigen3 existing on the system. Use vcpkg manager on windows to also utilize the CMakeSettings.json
option(test "Build all tests." ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mssa)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mssa)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/mssa)
message("Bin: ${CMAKE_BINARY_DIR}")

project(MSSAProcessingPipeline VERSION 1.0.0)
file(GLOB_RECURSE file_search ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.hpp)

list(REMOVE_ITEM file_search ${PROJECT_SOURCE_DIR}/src/mexEntry.cpp)


add_library(${PROJECT_NAME} STATIC ${file_search})
target_compile_definitions(${PROJECT_NAME} PRIVATE FILE_LOCATION=1)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
find_package(Eigen3 REQUIRED)


#include_directories(${EIGEN_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

# Requires that a version of Matlab be installed and found. If not found, add to PATH or Matlab_ROOT_DIR
find_package(Matlab REQUIRED)
if(Matlab_FOUND)
	target_link_libraries(${PROJECT_NAME} PRIVATE Matlab::MatlabEngine Matlab::MatlabDataArray)
	matlab_add_mex(NAME MSSAMex SHARED SRC ${PROJECT_SOURCE_DIR}/src/mexEntry.cpp LINK_TO ${PROJECT_NAME} Eigen3::Eigen Matlab::mex Matlab::mx)
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
message("Debug Mode")	
enable_testing()
add_subdirectory(tests)
else()
message("Release Mode")
#[[
install(TARGETS ${PROJECT_NAME} MSSAMex 
		RUNTIME DESTINATION .
		PUBLIC_HEADER DESTINATION lib)
include(CPack)

message("${EIGEN3_INCLUDE_DIRS}")
find_program(ZIP_EXECUTABLE 7z 
        PATHS "$ENV{ProgramFiles}/7-Zip")

message("${ZIP_EXECUTABLE}")
IF(ZIP_EXECUTABLE)
	message("Found 7zip")
    SET(ZIP_COMMAND "\"${ZIP_EXECUTABLE}\" a -tzip \"<ARCHIVE>\" @<FILELIST>")
ENDIF(ZIP_EXECUTABLE)
message("${ZIP_COMMAND}")
#]]

add_custom_command(TARGET MSSAMex
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} 
                   -DCMAKE_RELEASE_LOCATION=${PROJECT_SOURCE_DIR}/release 
                   -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                   -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
                   -DEIGEN3_INCLUDE_DIRS=${EIGEN3_INCLUDE_DIRS}/Eigen
                   -DDOC_DIR=${PROJECT_SOURCE_DIR}/docs
                   -P ${PROJECT_SOURCE_DIR}/zipfiles.cmake
)

endif()
