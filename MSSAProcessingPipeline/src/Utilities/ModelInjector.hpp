#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>
#include "../MSSA/MSSA.hpp"
#include <Eigen/Dense>
#ifdef _TORCH
	#include <torch/torch.h>
	#include <torch/script.h> // One-stop header.
using namespace torch::indexing;
#endif

using namespace std;
using namespace Processor;

namespace Utils {
	/// <summary>
	/// Singleton structure to load and utilize the decision model for reconstructing the input signals.
	/// NOTE FOR ALL FURTURE SINGLETONS: They must be defined inside any files generating a library or executable.
	/// -> Example in mexEntry.cpp
	/// </summary>
	class Injector
	{
	private:
		static Injector* instance;
		Injector() = default;

#ifdef _TORCH
		torch::jit::script::Module model;
#else
		int model;
#endif

	public:
		/// <summary>
		/// GetInstance of Singleton. Apparently this way is thread-safe
		/// </summary>
		/// <returns></returns>
		static Injector * GetInstance() {
			if (!instance)
				instance = new Injector();
			return instance;
		}


#ifdef _TORCH
		/// <summary>
		/// Finds the TorchScript file to load
		/// </summary>
		/// <param name="filename">Path to file</param>
		void LoadModel(string filename) {
			try {
				// Deserialize the ScriptModule from a file using torch::jit::load().
				instance->model = torch::jit::load(filename);
			}
			catch (const c10::Error& e) {
				std::cerr << "error loading the model\n";
			}
		}

		/// <summary>
		/// Function to Apply the Injector's model to an Eigen matrix. This is where the decision function is run to tell if a component is valuable to the signal.
		/// </summary>
		/// <param name="input">Reconstruction Matrix is the expected value, but it can be anything</param>
		std::vector<int> ApplyModel(MSSA::ReconstructionMatrix input, MSSA::ValidSignal inboard, MSSA::ValidSignal outboard, double alpha) {
			// Converts Eigen to PyTorch Tensor
			// Based on https://stackoverflow.com/questions/74231905/how-to-convert-eigen-martix-to-torch-tensor
			auto options = torch::TensorOptions().dtype(torch::kFloat32);
			std::vector<int> indexList = std::vector<int>();
			int idx = 0;
			for (auto row : input.colwise()) {
				try {
					std::vector<int64_t> dims = { 1, 1, input.rows() };
					torch::Tensor T = torch::from_blob(row.data(), dims, options).clone();
					std::vector<torch::jit::IValue> inputs;
					inputs.push_back(T);
					at::Tensor output = instance->model.forward(inputs).toTensor();
					/*
					### Convert labels into probabilities ###
					### (e.g., label as interference has  ###
					###  probability of 1 in interference ###
					###  column, 0 in nominal column)     ###
					*/
					auto value1 = output[0][0].item<float>();
					auto value2 = output[0][1].item<float>();
					auto accessor = output.accessor<float, 2>();
					auto test = accessor[0][1];
					if (value2 > alpha) {
						indexList.push_back(idx);
					}

				}
				catch (std::exception const& ex) {
					std::string error_message = "";
					error_message.append(ex.what());
					error_message.append("\nError occurred in correlation coefficient calculations.");
					throw std::invalid_argument(error_message.c_str());

				}
				idx++;
			}
			return indexList;
		}
#else
		/// <summary>
		/// Finds the TorchScript file to load
		/// </summary>
		/// <param name="filename">Path to file</param>
		void LoadModel(string filename) {
			cout << "No Loading Required." << endl;
		}

		/// <summary>
		/// Gets a list of indices that are required for reconstruction. Uses ComponentSelection process with Correlation Coefficient
		/// </summary>
		/// <param name="input"></param>
		/// <param name="inboard"></param>
		/// <param name="outboard"></param>
		/// <returns></returns>
		std::vector<int> ApplyModel(MSSA::ReconstructionMatrix recon, MSSA::ValidSignal inboard, MSSA::ValidSignal outboard, double alpha) {
			Eigen::Map<Eigen::MatrixXd> x(inboard.data(), 1, MSSA::InputSize());
			Eigen::Map<Eigen::MatrixXd> y(outboard.data(), 1, MSSA::InputSize());
			Eigen::MatrixXd interference = x - y;

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
				}
				catch (std::exception const& ex) {
					std::string error_message = "";
					error_message.append(ex.what());
					error_message.append("\nError occurred in correlation coefficient calculations.");
					throw std::invalid_argument(error_message.c_str());

				}

			}
			return indexList;

		}
#endif

	};

	
}