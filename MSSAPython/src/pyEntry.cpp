#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // for copy() and assign()�
#include <iterator> // for back_inserter�


#define STRINGIFY(x) #x
#define MACRO_STRINGIFY(x) STRINGIFY(x)
namespace py = pybind11;

/*
Function Description:
%	Function inputs are expected to be contained withing a Python Dictionary containing the following:
%		1. inboard_input: 3D magnetic field data in 3xN double value format
%		2. outboard_input: 3D magnetic field data in 3xN double value format
%		3. timestamp: 1D matrix of datetime values representing the time the magnetic field data record was recorded
%		4. dimensions: N in the NxM matrix provided to MSSAMex. Default to 3
%		5. alpha_threshold: Value between [0, 1] used to select components for reconstruction. Defaults to 0.05
%		6. segment_size: Size of the N magnetic field data points to use as for each analysis cycle.
%						 If the segment_size < N, the magnetic field data is broken up into N/segment_size-1 segments
%						 (exludes last segment). Defaults to 5000
%		7. window_size: Size of the window used by MSSA when deconstructing signal. Defaults to 40
%
%	Function outputs are in a Python Dictionary as follows:
%		1. inboard_result: Reconstructed 3D magnetic field data representing the Inboard data
%		2. outboard_result: Reconstructed 3D magnetic field data representing the Outboard data
%		3. flags: Flags representing how each element has been affected or considered.
%				  The current makeup is an 8-bit integer with each bit representing a flag.
%				  Flags from right to left are as follows:
%			a. Segment Start: value is the start of a new segment
%			b. Merged: value has been marked as being merged between 2 segments separately running MSSA
%			c. Skipped: value will be equal to the original input signal
%			d. Time Jump: value represents the start of a new segment due to a discontinuity in the timeline
%			e. NaN: value has been registered as a NaN
%			f. Wheel Error: value has not had the wheel removed. Affects all values of a segment
%		4. win: Wheel of inboard signal
%		5. wout: Wheel of outboard signal

*/


Utils::FlagSystem* Utils::FlagSystem::instance;
py::dict pyEntry(const py::dict &inputs) {
	using SignalProcessingUnit::MSSAProcessingUnit;
	using namespace pybind11::literals; // to bring in the `_a` literal
	// Initialize flags
	
	// Access lists from the dictionary
    py::array inboard_input = inputs["inboard_input"].cast<py::array>().attr("flatten")();;
    py::array outboard_input = inputs["outboard_input"].cast<py::array>().attr("flatten")();;
    py::array timestamp = inputs["timestamp"].cast<py::array>().attr("flatten")();;

	// Access other values from the dictionary
	unsigned int dimensions = inputs["dimensions"].cast<unsigned int>();
	double alpha_threshold = inputs["alpha_threshold"].cast<double>();
	int segment_size = inputs["segment_size"].cast<int>();


    /*
	* 

	// Check if the arguments are valid and throw an error if so
	bool errType = checkArguments(outputs, inputs, matlabPtr, factory);
	switch(errType){
		case -1:
			break;
		case -2:
			break;
		case -3:
			break;
		default:
			// Passing code
			std::cout << "Passed TSA checking" << std::endl;
	}
	*/
	
	/*
	try {
	

		unsigned int dimensions = 3;
		if (inputs.size() > 3) {
			matlab::data::TypedArray<double> dimen = std::move(inputs[3]);
			dimensions = std::round(dimen[0]);
		}
		if (inputs.size() > 5)
		{
			matlab::data::TypedArray<double> inputSize = std::move(inputs[5]);
			matlab::data::TypedArray<double> windowSize = std::move(inputs[6]);
			Processor::MSSA::DynamicVariableSetup(inputSize[0], windowSize[0]);
		}

	MSSAProcessingUnit<double> inboard = MSSAProcessingUnit<double>(true, dimensions);
	MSSAProcessingUnit<double> outboard = MSSAProcessingUnit<double>(false, dimensions);

	Utils::FlagSystem::GetInstance()->Resize(dest.size());
	Utils::FlagSystem::GetInstance()->FlagNaN(dest);
	Utils::FlagSystem::GetInstance()->FlagNaN(dest2);

	if (inputs[2].getType() != matlab::data::ArrayType::DOUBLE) {
		matlab::data::Array time = std::move(inputs[2]);
		matlab::data::TypedArray<double> timenum = matlabPtr->feval(u"datenum", time);
		std::vector<double> timevec(timenum.begin(), timenum.end());
		Utils::FlagSystem::GetInstance()->FlagDiscontinuity(timevec);
	}
	else {
		matlab::data::TypedArray<double> timenum = std::move(inputs[2]);
		std::vector<double> timevec(timenum.begin(), timenum.end());
		Utils::FlagSystem::GetInstance()->FlagDiscontinuity(timevec);
	}

	inboard.PreProcess(dest, true);
	outboard.PreProcess(dest2, true);

	std::vector<double> alpha_val = { 0.05 };
	if (inputs.size() > 4) {
		matlab::data::TypedArray<double> alpha = std::move(inputs[4]);
		alpha_val.clear();
		copy(alpha.begin(), alpha.end(), back_inserter(alpha_val));
	}
	MSSAProcessingUnit<double>::Process(inboard, outboard, alpha_val);

	auto temp = outboard.JoinWheel();
	auto wheel = inboard.JoinWheel();
	outputs[3] = factory.createArray({ dimensions, wheel.size() / dimensions }, wheel.begin(), wheel.end());

	temp = outboard.JoinWheel();
	outputs[4] = factory.createArray({ dimensions, temp.size() / dimensions }, temp.begin(), temp.end());

	auto signal = inboard.JoinSignal(dest);
	outputs[0] = factory.createArray({ dimensions, signal.size() / dimensions }, signal.begin(), signal.end());
	inboard.CheckSignalDifference(signal, wheel, dest);

	signal = outboard.JoinSignal(dest2);
	outputs[1] = factory.createArray({ dimensions, signal.size() / dimensions }, signal.begin(), signal.end());
	outboard.CheckSignalDifference(signal, temp, dest2);

	std::vector<int> temp2 = Utils::FlagSystem::GetInstance()->Snapshot();
	outputs[2] = factory.createArray({ dimensions, temp2.size() / dimensions }, temp2.begin(), temp2.end());

	}
	catch (const matlab::engine::MATLABException& ex) {
		matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar("Datenum function call didn't work. Likely error with time object.") }));
		matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar(ex.what()) }));
	}
	catch (std::exception const& ex) {
		matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar(ex.what()) }));
	}

			*/
	py::dict outputDict("spam"_a = py::none(), "eggs"_a = 42);
	return outputDict;
	
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

//#ifdef VERSION_INFO
//	m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
//#else
	m.attr("__version__") = "dev";
//#endif
}