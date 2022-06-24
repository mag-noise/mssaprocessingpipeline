# CMake files for project GTest framework
cmake_minimum_required(VERSION 3.15)

if(test)
	project(ProcessingTests VERSION 1.0.0)
	get_filename_component(PROJECT_PARENT ${PROJECT_SOURCE_DIR} DIRECTORY)
	file(GLOB_RECURSE file_search ${PROJECT_PARENT}/src/*.cpp ${PROJECT_PARENT}/src/*.hpp)
	list(REMOVE_ITEM file_search ${PROJECT_PARENT}/src/main.cpp ${PROJECT_PARENT}/src/mexEntry.cpp)

	add_executable(${PROJECT_NAME} ${file_search} ${PROJECT_SOURCE_DIR}/Test.cpp ${PROJECT_SOURCE_DIR}/TestSpu.cpp ${PROJECT_SOURCE_DIR}/TestFlags.cpp)
	include_directories(${GTEST_INCLUDE_DIRS})
	find_package(GTest REQUIRED)
	find_package(Eigen3 REQUIRED NO_MODULE)
	find_package(Matlab REQUIRED)

	include_directories(${EIGEN_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen Matlab::MatlabEngine Matlab::MatlabDataArray GTest::gtest GTest::Main)
	include(GoogleTest)
	gtest_discover_tests(${PROJECT_NAME})
	add_compile_definitions(_MAT_ _DEBUG)
	#add_test(NAME CorrectlyReadsCSVFile COMMAND ${PROJECT_NAME})
	#add_test(NAME RecreatesOriginalRandom COMMAND ${PROJECT_NAME})
	#add_test(NAME RecreatesOriginal COMMAND ${PROJECT_NAME})
	#add_test(NAME SignalOutputSimilar COMMAND ${PROJECT_NAME})
	#add_test(NAME ReadMatfile COMMAND ${PROJECT_NAME})
	#add_test(NAME MatFileProcessing COMMAND ${PROJECT_NAME})

endif()