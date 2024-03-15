#include <pybind11/pybind11.h>

#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // for copy() and assign() 
#include <iterator> // for back_inserter 


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
namespace py = pybind11;

Utils::FlagSystem* Utils::FlagSystem::instance;
void pyEntry(py::dict inputs) {
	using SignalProcessingUnit::MSSAProcessingUnit;
    return;
	// Initialize flags
	
}

// Registering entry function with Pybind11
PYBIND11_MODULE(MSSAPython, m) {
	m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------

        .. currentmodule:: scikit_build_example

        .. autosummary::
           :toctree: _generate

           pyEntry
    )pbdoc";

	m.def("pyEntry", &pyEntry, R"pbdoc(
        Add two numbers

        Some other explanation about the add function.
    )pbdoc");

#ifdef VERSION_INFO
	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
	m.attr("__version__") = "dev";
#endif
}