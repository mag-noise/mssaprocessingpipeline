#include "mex.hpp"
#include "mexAdapter.hpp"
#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace matlab::data;

class MexFunction : public matlab::mex::Function {
public:
	void operator() (matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
		using SignalProcessingUnit::MSSAProcessingUnit;
		checkArguments(outputs, inputs);
		MSSAProcessingUnit<double> inboard = MSSAProcessingUnit<double>(true);
		MSSAProcessingUnit<double> outboard = MSSAProcessingUnit<double>(false);
		matlab::data::TypedArray<double> in = std::move(inputs[0]);
		std::vector<double> dest(in.begin(), in.end());
		inboard.PreProcess(dest, true);

		matlab::data::TypedArray<double> out = std::move(inputs[1]);
		std::vector<double> dest2(out.begin(), out.end());
		outboard.PreProcess(dest2, true);

		MSSAProcessingUnit<double>::Process(inboard, outboard);
		matlab::data::ArrayFactory factory;

		auto temp = inboard.Join();
		outputs[0] = factory.createArray({ 3, temp.size() / 3 }, temp.begin(), temp.end());

		temp = outboard.Join();
		outputs[1] = factory.createArray({ 3, temp.size() / 3 }, temp.begin(), temp.end());
	}

	void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
		std::shared_ptr<matlab::engine::MATLABEngine> matPtr = getEngine();
		matlab::data::ArrayFactory factory;
		// Error message template:
		if(inputs[0].getType() != ArrayType::DOUBLE && inputs[1].getType() != ArrayType::DOUBLE)
			matPtr->feval(u"error", 0, std::vector<Array>({ factory.createScalar("Input should be inboard and outboard.") }));
	}
};