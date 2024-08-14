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
# 
find_package(Python COMPONENTS Interpreter Development)
if(Python_FOUND)
    message("${Python_LIBRARY_DIRS}")
    message("${Python_VERSION}")
    message("${MSSAProcessingPipeline_SOURCE_DIR}")

    set(PYBIND11_NEWPYTHON ON)
    set(PYBIND11_NOPYTHON FALSE)
    find_package(pybind11 CONFIG REQUIRED)
    # pybind11 method:
    pybind11_add_module(MSSAPython  src/pyEntry.cpp)

    # Add a library using FindPython's tooling (pybind11 also provides a helper like
    # this)
    #Python_add_library(MSSAPython src/pyEntry.cpp)

    set_target_properties(MSSAPython PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION ON
        CXX_VISIBILITY_PRESET ON
        VISIBILITY_INLINES_HIDDEN ON
    )


    target_link_libraries(MSSAPython PRIVATE pybind11::headers)
    target_include_directories(MSSAPython PUBLIC ${MSSAProcessingPipeline_SOURCE_DIR}/MSSAProcessingPipeline/src)
    target_link_libraries(MSSAPython PUBLIC sub::mssaLib)

    # This is passing in the version as a define just as an example
    target_compile_definitions(MSSAPython PRIVATE VERSION_INFO=${PROJECT_VERSION})

    # The install directory is the output (wheel) directory
    install(TARGETS MSSAPython DESTINATION MSSAPython)
    
    message("Installed Python Library")
    # detect virtualenv and set Pip args accordingly
    
    if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
      set(_pip_args)
    else()
      set(_pip_args "--user")
    endif()
    #message("Executing Build of Python")
    #execute_process(COMMAND pipx run build --wheel WORKING_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    message("Executing Install of Python")
    execute_process(COMMAND ${Python_EXECUTABLE} -m pip install ${_pip_args} ${_pypkg})
endif()
