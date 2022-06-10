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
private:
	//std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr;

public:
	void operator() (matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
		using SignalProcessingUnit::MSSAProcessingUnit;
		checkArguments(outputs, inputs);
		
		matlab::data::TypedArray<double> inputSize = std::move(inputs[3]);
		matlab::data::TypedArray<double> windowSize = std::move(inputs[4]);


		/*PLACEHOLDER: REMOVE STATIC VALUES AFTER IMPLEMENTING PATH*/
		Processor::MSSA::DynamicVariableSetup(/*Input Size*/inputSize[0], /*Window Size*/windowSize[0]);


		MSSAProcessingUnit<double> inboard = MSSAProcessingUnit<double>(true);
		MSSAProcessingUnit<double> outboard = MSSAProcessingUnit<double>(false);
		matlab::data::TypedArray<double> in = std::move(inputs[0]);

		//std::ostringstream stream;
		//stream << "Inboard 0: " << double(in[0]) << std::endl;
		//stream << "test" << std::endl;

		//displayOnMATLAB(stream);
		std::vector<double> dest(in.begin(), in.end());
		inboard.PreProcess(dest, true);

		matlab::data::TypedArray<double> out = std::move(inputs[1]);
		std::vector<double> dest2(out.begin(), out.end());
		outboard.PreProcess(dest2, true);
		
		matlab::data::TypedArray<double> alpha = std::move(inputs[2]);
		double alpha_val = alpha[0];

		MSSAProcessingUnit<double>::Process(inboard, outboard, alpha[0]);
		matlab::data::ArrayFactory factory;

		auto temp = inboard.Join();
		outputs[0] = factory.createArray({ 3, temp.size() / 3 }, temp.begin(), temp.end());

		temp = outboard.Join();
		outputs[1] = factory.createArray({ 3, temp.size() / 3 }, temp.begin(), temp.end());
	}

	void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
		std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr;
		matlabPtr = getEngine();
		matlab::data::ArrayFactory factory;
		// Error message template:
		if(inputs[0].getType() != ArrayType::DOUBLE && inputs[1].getType() != ArrayType::DOUBLE)
			matlabPtr->feval(u"error", 0, std::vector<Array>({ factory.createScalar("Input should be inboard and outboard.") }));
		
	}
	void displayOnMATLAB(std::ostringstream& stream) {
		// Pass stream content to MATLAB fprintf function
		std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr;
		matlabPtr = getEngine();
		matlab::data::ArrayFactory factory;
		matlabPtr->feval(u"fprintf", 0,
			std::vector<Array>({ factory.createScalar(stream.str()) }));
		// Clear stream buffer
		stream.str("");
	}
};