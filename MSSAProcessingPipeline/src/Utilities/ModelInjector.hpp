#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>
//#include <torch/csrc/api/include/torch/torch.h>
#ifdef _TORCH
	#include <torch/torch.h>
#endif

namespace Utils {
	using namespace std;
	/// <summary>
	/// Structure to organize use of gradient reduction within intersecting segments
	/// </summary>
	struct Injector
	{
#ifdef _TORCH
	private:
		torch::Tensor model;
	public:

		Injector() {
			model = torch::rand({ 2, 3 });
		}

		Injector(std::string fileLocation) {
			model = torch::rand({ 2, 3 });
		}
#else
	private:
		int model;
	public:
		Injector() {
			model = 0;
		}

		Injector(std::string fileLocation) {
			model = 1;
		}
#endif
		void HelloWorld();

	};

	inline void Injector::HelloWorld() {
		std::cout << model << std::endl;
	}

	
}