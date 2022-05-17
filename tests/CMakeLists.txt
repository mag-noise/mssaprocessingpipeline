# CMake files for project GTest framework
cmake_minimum_required(VERSION 3.15)

if(test)
	project(ProcessingTests VERSION 0.1.0)
	get_filename_component(PROJECT_PARENT ${PROJECT_SOURCE_DIR} DIRECTORY)
	file(GLOB_RECURSE file_search ${PROJECT_PARENT}/src/*.cpp ${PROJECT_PARENT}/src/*.hpp)
	list(REMOVE_ITEM file_search ${PROJECT_PARENT}/src/main.cpp)

	enable_testing()
	add_executable(${PROJECT_NAME} ${file_search} ${PROJECT_SOURCE_DIR}/Test.cpp ${PROJECT_SOURCE_DIR}/TestCpu.cpp ${PROJECT_SOURCE_DIR}/TestProcessor.cpp)

	find_package(gtest REQUIRED)
	find_package(Eigen3 REQUIRED NO_MODULE)
	find_package(Matlab REQUIRED)
	#set(matlab_versions)
	#matlab_extract_all_installed_versions_from_registry(ON matlab_versions)
	#matlab_get_all_valid_matlab_roots_from_registry(matlab_versions, matlab_roots)
	#message(STATUS " !!!!! ${matlab_versions} ${matlab_roots} ${Matlab_ROOT_DIR} ####")
	#include_directories("${Matlab_ROOT_DIR}/extern/bin/win64")

	target_link_libraries(${PROJECT_NAME} PRIVATE Eigen3::Eigen Matlab::MatlabEngine Matlab::MatlabDataArray GTest::gtest)
	#include(GoogleTest)
	#gtest_discover_tests(${PROJECT_NAME})

	add_test(NAME CorrectlyReadsCSVFile COMMAND ${PROJECT_NAME})
	add_test(NAME RecreatesOriginalRandom COMMAND ${PROJECT_NAME})
	add_test(NAME RecreatesOriginal COMMAND ${PROJECT_NAME})
	add_test(NAME SignalOutputSimilar COMMAND ${PROJECT_NAME})
	add_test(NAME ReadMatfile COMMAND ${PROJECT_NAME})

endif()