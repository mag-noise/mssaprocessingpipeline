project(MSSAProcessingPipeline VERSION 1.2.0)
file(GLOB_RECURSE file_search ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.hpp)

list(REMOVE_ITEM file_search ${PROJECT_SOURCE_DIR}/src/mexEntry.cpp)
list(REMOVE_ITEM file_search ${PROJECT_SOURCE_DIR}/src/pyEntry.cpp)

add_library(${PROJECT_NAME} STATIC ${file_search})
add_library(sub::mssaLib ALIAS ${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME} PRIVATE FILE_LOCATION=1)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

message("Prefix: ${CMAKE_PREFIX_PATH}")
if(DEFINED ENV{EIGEN3_INCLUDE_DIR})
    include_directories("$ENV{EIGEN3_INCLUDE_DIR}")
    set(EIGEN3_INCLUDE_DIR "$ENV{EIGEN3_INCLUDE_DIR}")
else()
    find_package(Eigen3 REQUIRED)
endif()

#include_directories(${EIGEN_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen)

# Requires that a version of Matlab be installed and found. If not found, add to PATH or Matlab_ROOT_DIR
find_package(Matlab)
if(Matlab_FOUND)
	target_link_libraries(${PROJECT_NAME} PRIVATE Matlab::MatlabEngine Matlab::MatlabDataArray)
    matlab_add_mex(NAME MSSAMex SHARED SRC ${PROJECT_SOURCE_DIR}/src/mexEntry.cpp LINK_TO ${PROJECT_NAME} Eigen3::Eigen Matlab::mex Matlab::mx DOCUMENTATION ${PROJECT_SOURCE_DIR}/docs/MSSAMex.txt)    
endif()


if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message("Debug Mode")	
    enable_testing()
    add_subdirectory(tests)
endif()