project(MSSAPython 
        VERSION 0.1.0
        LANGUAGES CXX)


if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message("Debug Mode")	
    enable_testing()
    set(SKBUILD_CMAKE_BUILD_TYPE Debug)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND Matlab_FOUND)
    message("Release Mode")
    set(SKBUILD_CMAKE_BUILD_TYPE Release)
endif()

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
if(Python_FOUND)
    message("${Python_LIBRARY_DIRS}/python310_d.lib")

    set(PYBIND11_NEWPYTHON ON)
    find_package(pybind11 CONFIG REQUIRED)
    # Add a library using FindPython's tooling (pybind11 also provides a helper like
    # this)

    python_add_library(MSSAPython MODULE src/pyEntry.cpp WITH_SOABI)

    target_link_libraries(MSSAPython PRIVATE pybind11::headers)
    target_include_directories(MSSAPython PUBLIC ${MSSAProcessingPipeline_SOURCE_DIR}/src)
    target_link_libraries(MSSAPython PUBLIC sub::mssaLib)

    # This is passing in the version as a define just as an example
    target_compile_definitions(MSSAPython PRIVATE VERSION_INFO=${PROJECT_VERSION})

    # The install directory is the output (wheel) directory
    install(TARGETS MSSAPython DESTINATION MSSAPython)
    message("Installed Python Library")

endif()
