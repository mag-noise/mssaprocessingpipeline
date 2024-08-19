#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>
//#include <torch/csrc/api/include/torch/torch.h>
#ifdef _TORCH
	#include <torch/torch.h>
	#include <torch/script.h> // One-stop header.
#endif
#include "../MSSA/MSSA.hpp"
#include <Eigen/Dense>


namespace Utils {
	using namespace std;
	using namespace Processor;
	/// <summary>
	/// Structure to load and utilize the decision model for reconstructing the input signals
	/// </summary>
	struct Injector
	{
#ifdef _TORCH
	private:
		torch::jit::script::Module model;
	public:

		Injector() {
		}
#else
	private:
		int model;
	public:
		Injector() {
			model = 0;
		}
#endif
		void HelloWorld();
		void LoadModel(string filename);
		std::vector<int> ApplyModel(MSSA::ReconstructionMatrix, MSSA::ValidSignal inboard, MSSA::ValidSignal outboard, double alpha);

	};

	inline void Injector::HelloWorld() {
		std::cout << "Hello World" << std::endl;
	}

#ifdef _TORCH
	/// <summary>
	/// Finds the TorchScript file to load
	/// </summary>
	/// <param name="filename">Path to file</param>
	inline void Injector::LoadModel(string filename) {
		try {
			// Deserialize the ScriptModule from a file using torch::jit::load().
			model = torch::jit::load(filename);
		}
		catch (const c10::Error& e) {
			std::cerr << "error loading the model\n";
		}
	}

	/// <summary>
	/// Function to Apply the Injector's model to an Eigen matrix. This is where the decision function is run to tell if a component is valuable to the signal.
	/// </summary>
	/// <param name="input">Reconstruction Matrix is the expected value, but it can be anything</param>
	inline std::vector<int> Injector::ApplyModel(MSSA::ReconstructionMatrix input, MSSA::ValidSignal inboard, MSSA::ValidSignal outboard, double alpha) {
		// Converts Eigen to PyTorch Tensor
		// Based on https://stackoverflow.com/questions/74231905/how-to-convert-eigen-martix-to-torch-tensor
		auto options = torch::TensorOptions().dtype(torch::kFloat32);
		for(auto row : input.rowwise()){
			std::vector<int64_t> dims = {1, 1, input.cols() };
			torch::Tensor T = torch::from_blob(row.data(), dims, options).clone();
			std::vector<torch::jit::IValue> inputs;
			inputs.push_back(T);
			at::Tensor output = model.forward(inputs).toTensor();
		}
		std::vector<int> a(10,0);
		return  a;
	}
#else
	/// <summary>
	/// Finds the TorchScript file to load
	/// </summary>
	/// <param name="filename">Path to file</param>
	inline void Injector::LoadModel(string filename) {
		cout << "No Loading Required." << endl;
	}

	/// <summary>
	/// Gets a list of indices that are required for reconstruction. Uses ComponentSelection process with Correlation Coefficient
	/// </summary>
	/// <param name="input"></param>
	/// <param name="inboard"></param>
	/// <param name="outboard"></param>
	/// <returns></returns>
	inline std::vector<int> Injector::ApplyModel(MSSA::ReconstructionMatrix input, MSSA::ValidSignal inboard, MSSA::ValidSignal outboard, double alpha) {
		Eigen::Map<Eigen::MatrixXd> x(inboard.data(), 1, InputSize());
		Eigen::Map<Eigen::MatrixXd> y(outboard.data(), 1, InputSize());
		MatrixXd interference = x - y;

		std::vector<int> indexList = std::vector<int>();
		std::vector<double> listOfCheckValues = std::vector<double>();
		//std::vector<double> listOfCheckValuesX = std::vector<double>();
		//std::vector<double> listOfCheckValuesY = std::vector<double>();

		for (int i = 0; i < recon.cols(); i++) {
			try {
				auto check = MSSA::CorrelationCoefficient(recon.col(i), interference);
				auto checkX = MSSA::CorrelationCoefficient(recon.col(i), x);
				auto checkY = MSSA::CorrelationCoefficient(recon.col(i), y);
				if ((abs(check) > alpha && abs(check) > abs(checkX) && abs(check) > abs(checkY)) ||
					(abs(abs(check) - abs(checkX)) < alpha) || abs(abs(check) - abs(checkY)) < alpha) {
					indexList.push_back(i);
				}
				listOfCheckValues.push_back(check);
				//listOfCheckValuesX.push_back(checkX);
				//listOfCheckValuesY.push_back(checkY);
			}
			catch (std::exception const& ex) {
				std::string error_message = "";
				error_message.append(ex.what());
				error_message.append("\nError occurred in correlation coefficient calculations.");
				throw std::invalid_argument(error_message.c_str());

			}

		}
		//indexList = { 0, 1, 40, 41 };
		return indexList;
		
	}
#endif

	
}