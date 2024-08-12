#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>
//#include <torch/csrc/api/include/torch/torch.h>
#include <torch/torch.h>

namespace Utils {
	using namespace std;
	/// <summary>
	/// Structure to organize use of gradient reduction within intersecting segments
	/// </summary>
	struct Injector
	{
	private:
		torch::Tensor model;
	public:

		Injector() {
			model = torch::rand({ 2, 3 });
		}

		Injector(std::string fileLocation) {
			model = torch::rand({ 2, 3 });
		}

		void HelloWorld();

	};

	inline void Injector::HelloWorld() {
		std::cout << model << std::endl;
	}

	
}