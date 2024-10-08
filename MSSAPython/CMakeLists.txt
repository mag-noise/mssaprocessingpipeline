project(MSSAPython LANGUAGES CXX)


if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    message("Debug Mode")	
    enable_testing()
    set(SKBUILD_CMAKE_BUILD_TYPE Debug)
    set(Py_DEBUG TRUE)
elseif("${CMAKE_BUILD_TYPE}" STREQUAL "Release" AND Matlab_FOUND)
    message("Release Mode")
    set(SKBUILD_CMAKE_BUILD_TYPE Release)
endif()


# If a specific version of python is required, then use the following or provide with
# command line cmake call:
# set(Python3_ROOT_DIR <python.exe root dir>)
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
if(Python3_FOUND)
    message("${Python_LIBRARY_DIRS}")
    message("${Python_VERSION}")

    set(PYBIND11_NEWPYTHON ON)
    set(PYBIND11_NOPYTHON FALSE)
    find_package(pybind11 CONFIG REQUIRED)
    # Add a library using FindPython's tooling (pybind11 also provides a helper like
    # this)
    python3_add_library(MSSAPython MODULE src/pyEntry.cpp WITH_SOABI)

    target_link_libraries(MSSAPython PRIVATE pybind11::headers)
    target_include_directories(MSSAPython PUBLIC ${MSSAProcessingPipeline_SOURCE_DIR}/src)
    target_link_libraries(MSSAPython PUBLIC sub::mssaLib)

    # This is passing in the version as a define just as an example
    target_compile_definitions(MSSAPython PRIVATE VERSION_INFO=${PROJECT_VERSION})

    # The install directory is the output (wheel) directory
    install(TARGETS MSSAPython DESTINATION MSSAPython)
    message("Installed Python Library")

endif()
