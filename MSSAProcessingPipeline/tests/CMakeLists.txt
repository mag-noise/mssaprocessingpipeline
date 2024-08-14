# CMake files for project GTest framework
cmake_minimum_required(VERSION 3.15)
if(test)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	project(ProcessingTests VERSION 1.0.0)
	
	include(FetchContent)
	FetchContent_Declare(
	  googletest
	  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
	)
	# For Windows: Prevent overriding the parent project's compiler/linker settings
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)

	get_filename_component(PROJECT_PARENT ${PROJECT_SOURCE_DIR} DIRECTORY)
	file(GLOB_RECURSE file_search ${PROJECT_PARENT}/src/*.cpp ${PROJECT_PARENT}/src/*.hpp)
	list(REMOVE_ITEM file_search ${PROJECT_PARENT}/src/main.cpp ${PROJECT_PARENT}/src/mexEntry.cpp)
	file(GLOB_RECURSE test_search ${PROJECT_SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/*.hpp)
	#list(REMOVE_ITEM test_search ${PROJECT_SOURCE_DIR}/TestInjector.cpp)
	add_executable(${PROJECT_NAME} ${file_search} ${test_search})
	find_package(GTest REQUIRED)
	find_package(Eigen3 REQUIRED NO_MODULE)
	find_package(Matlab REQUIRED)

	# Required find and definition for Torch on Windows systems
	# Must be defined and linked first before any other libraries to work
	find_package(mimalloc REQUIRED)
	find_package(Torch REQUIRED)

	include_directories(SYSTEM ${TORCH_INCLUDE_DIRS})
	include_directories(${EIGEN_INCLUDE_DIRS})
	include_directories(${GTEST_INCLUDE_DIRS})

	target_link_libraries(${PROJECT_NAME} mimalloc)
	target_link_libraries(${PROJECT_NAME} GTest::gtest GTest::Main)
	target_link_libraries(${PROJECT_NAME} Eigen3::Eigen Matlab::MatlabEngine Matlab::MatlabDataArray )
	target_link_libraries(${PROJECT_NAME} "${TORCH_LIBRARIES}")
	
	include(GoogleTest)
	
	# Next line needed to find GTest's Qt DLLs apparently
	# Fix found here https://discourse.cmake.org/t/cmake-build-and-test-with-qt-and-google-test/4729
	set(CMAKE_GTEST_DISCOVER_TESTS_DISCOVERY_MODE PRE_TEST)
	
	gtest_discover_tests(${PROJECT_NAME})
    
	add_compile_definitions(GLOG_USE_GLOG_EXPORT)
	#add_compile_definitions(_MAT_)
	add_compile_definitions(_TEST)
	
	
	#add_test(NAME CorrectlyReadsCSVFile COMMAND ${PROJECT_NAME})
	#add_test(NAME RecreatesOriginalRandom COMMAND ${PROJECT_NAME})
	#add_test(NAME RecreatesOriginal COMMAND ${PROJECT_NAME})
	#add_test(NAME SignalOutputSimilar COMMAND ${PROJECT_NAME})
	#add_test(NAME ReadMatfile COMMAND ${PROJECT_NAME})
	#add_test(NAME MatFileProcessing COMMAND ${PROJECT_NAME})

endif()