#include "MSSA/MSSA.hpp"
#include "SPU/SPU.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm> // for copy() and assign() 
#include <iterator> // for back_inserter 
#include <stdexcept>

#include "mex.hpp"
#include "mexAdapter.hpp"
Utils::FlagSystem* Utils::FlagSystem::instance;
Utils::Injector* Utils::Injector::instance;
class MexFunction : public matlab::mex::Function {
private:
	//std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr;
public:
	void operator() (matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs) {
		using SignalProcessingUnit::MSSAProcessingUnit;

		// Initialize flags

		std::shared_ptr<matlab::engine::MATLABEngine> matlabPtr;
		matlabPtr = getEngine();
		matlab::data::ArrayFactory factory;
		checkArguments(outputs, inputs, matlabPtr, factory);
		
		try {

			matlab::data::TypedArray<double> in = std::move(inputs[0]);
			std::vector<double> dest(in.begin(), in.end());

			matlab::data::TypedArray<double> out = std::move(inputs[1]);
			std::vector<double> dest2(out.begin(), out.end());

			unsigned int dimensions = 3;
			if (inputs.size() > 3) {
				matlab::data::TypedArray<double> dimen = std::move(inputs[3]);
				dimensions = std::round(dimen[0]);
			}
			if (inputs.size() > 5)
			{
				matlab::data::TypedArray<double> inputSize = std::move(inputs[5]);
				matlab::data::TypedArray<double> windowSize = std::move(inputs[6]);
				if (inputSize[0] <= 0) {
					inputSize[0] = floor(dest.size() / dimensions);
				}
				Processor::MSSA::DynamicVariableSetup(/*Input Size*/inputSize[0], /*Window Size*/windowSize[0]);
			}
			if (inputs.size() > 7) {
				Utils::Injector* injector = Utils::Injector::GetInstance();
				//matlab::data::TypedArray<matlab::data::MATLABString> inArrayRef1 = std::move(inputs[7]);
				//std::string modelLocation = std::string(inArrayRef1[0]);
				//injector.LoadModel(modelLocation);
				injector->LoadModel("C:\\Users\\klsteele\\source\\repos\\mssaprocessingpipeline\\MSSAProcessingPipeline\\models\\FCN_20E_20240614_120751.pt");
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
				if (timevec.size() != dest.size() / 3 && timevec.size() != dest2.size() / 3) {
					throw std::invalid_argument("Error: input time matrix size must be equal to the input signal matrices");
				}
			}
			else {
				matlab::data::TypedArray<double> timenum = std::move(inputs[2]);
				std::vector<double> timevec(timenum.begin(), timenum.end());
				Utils::FlagSystem::GetInstance()->FlagDiscontinuity(timevec);
				if (timevec.size() != dest.size() / 3 && timevec.size() != dest2.size() / 3) {
					throw std::invalid_argument("Error: input time matrix size must be equal to the input signal matrices");
				}
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

			// Due to how the failed_wheel is found in the program, this ensures that the flag is correctly raised prior 
			// to committing joined containers
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
			matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar("Datenum function call didn't work. Likely error with time object.")}));
			matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar(ex.what()) }));
		}
		catch (std::exception const& ex) {
			matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar(ex.what()) }));
		}
	}

	void checkArguments(matlab::mex::ArgumentList outputs, matlab::mex::ArgumentList inputs, std::shared_ptr<matlab::engine::MATLABEngine> &matlabPtr, matlab::data::ArrayFactory& factory) {
		// Error message template:
		if(inputs.size() < 3)
			matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar("Input should be at least inboard, outboard, and timeseries.") }));
		if(inputs[0].getType() != matlab::data::ArrayType::DOUBLE && inputs[1].getType() != matlab::data::ArrayType::DOUBLE)
			matlabPtr->feval(u"error", 0, std::vector<matlab::data::Array>({ factory.createScalar("Required inputs should be matrices of double precision.") }));
		
	}
	void displayOnMATLAB(std::ostringstream& stream, std::shared_ptr<matlab::engine::MATLABEngine>& matlabPtr, matlab::data::ArrayFactory& factory) {
		// Pass stream content to MATLAB fprintf function
		matlabPtr->feval(u"fprintf", 0,
			std::vector<matlab::data::Array>({ factory.createScalar(stream.str()) }));
		// Clear stream buffer
		stream.str("");
	}
};