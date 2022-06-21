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
		std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr;
		matlabPtr = getEngine();
		matlab::data::ArrayFactory factory;
		checkArguments(outputs, inputs, matlabPtr, factory);
		
		try {

			if (inputs.size() > 4)
			{
				matlab::data::TypedArray<double> inputSize = std::move(inputs[4]);
				matlab::data::TypedArray<double> windowSize = std::move(inputs[5]);
				Processor::MSSA::DynamicVariableSetup(/*Input Size*/inputSize[0], /*Window Size*/windowSize[0]);
			}

			matlab::data::Array time = std::move(inputs[2]);
			matlab::data::TypedArray<double> timenum = matlabPtr->feval(u"datenum", time);
			std::vector<double> timevec(timenum.begin(), timenum.end());
			std::ostringstream stream;
			stream << "Timeseries Dimensions: " << timenum.getDimensions()[0] << ", " << timenum.getDimensions()[1] << std::endl;
			stream << "Timeseries first value: " << timevec[0] << std::endl;
			//matlabPtr->feval(u"disp", 0, std::vector<Array>({ factory.createScalar(time.getNumberOfElements()) }));
			
			//stream << "test" << std::endl;
			displayOnMATLAB(stream, matlabPtr, factory);

			MSSAProcessingUnit<double> inboard = MSSAProcessingUnit<double>(true);
			MSSAProcessingUnit<double> outboard = MSSAProcessingUnit<double>(false);
			matlab::data::TypedArray<double> in = std::move(inputs[0]);



			std::vector<double> dest(in.begin(), in.end());
			inboard.PreProcess(dest, true);

			matlab::data::TypedArray<double> out = std::move(inputs[1]);
			std::vector<double> dest2(out.begin(), out.end());
			outboard.PreProcess(dest2, true);
		
			double alpha_val = 0.05;
			if (inputs.size() > 3) {
				matlab::data::TypedArray<double> alpha = std::move(inputs[3]);
				alpha_val = alpha[0];
			}
			MSSAProcessingUnit<double>::Process(inboard, outboard, alpha_val);

			auto temp = inboard.Join();
			outputs[0] = factory.createArray({ 3, temp.size() / 3 }, temp.begin(), temp.end());

			temp = outboard.Join();
			outputs[1] = factory.createArray({ 3, temp.size() / 3 }, temp.begin(), temp.end());

		}
		catch (const matlab::engine::MATLABException& ex) {

			matlabPtr->feval(u"error", 0, std::vector<Array>({ factory.createScalar(ex.what()) }));
		}
	}

	void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs, std::shared_ptr<matlab::engine::MATLABEngine> &matlabPtr, matlab::data::ArrayFactory& factory) {
		// Error message template:
		if(inputs.size() < 3)
			matlabPtr->feval(u"error", 0, std::vector<Array>({ factory.createScalar("Input should be at least inboard, outboard, and timeseries.") }));
		if(inputs[0].getType() != ArrayType::DOUBLE && inputs[1].getType() != ArrayType::DOUBLE)
			matlabPtr->feval(u"error", 0, std::vector<Array>({ factory.createScalar("Required inputs should be matrices of double precision.") }));
		
	}
	void displayOnMATLAB(std::ostringstream& stream, std::shared_ptr<matlab::engine::MATLABEngine>& matlabPtr, matlab::data::ArrayFactory& factory) {
		// Pass stream content to MATLAB fprintf function
		matlabPtr->feval(u"fprintf", 0,
			std::vector<Array>({ factory.createScalar(stream.str()) }));
		// Clear stream buffer
		stream.str("");
	}
};