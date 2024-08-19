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
#include <Eigen/Dense>

namespace Utils {
	using namespace std;
	/// <summary>
	/// Structure to organize use of gradient reduction within intersecting segments
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
		void ApplyModel(Eigen::MatrixXd);

	};

	inline void Injector::HelloWorld() {
		std::cout << "Hello World" << std::endl;
	}
	inline void Injector::LoadModel(string filename) {
		try {
			// Deserialize the ScriptModule from a file using torch::jit::load().
			model = torch::jit::load(filename);
		}
		catch (const c10::Error& e) {
			std::cerr << "error loading the model\n";
		}
	}
	inline void Injector::ApplyModel(Eigen::MatrixXd input) {
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
	}


	
}