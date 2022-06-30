#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <functional>



namespace Utils {
	struct Gradients
	{
	private:
		std::vector<double> grads;
		std::function<double(int)> reduce;
		std::function<double(double, int)> stepper;
		double step;
	public:
		Gradients(std::size_t init_size) {
			grads = std::vector<double>(init_size, 1);
			step = 0;
		}
		void SetReduce(std::function<double(int)>);
		void SetStepper(std::function<double(double, int)>);
		double ReduceGrad(int idx, int size);
		void Reset();
		double operator[](int idx) {
			return grads[idx];
		}
	};

	inline void Gradients::SetReduce(std::function<double(int)> a) {
		reduce=a;
	}

	inline void Gradients::SetStepper(std::function<double(double, int)> a) {
		stepper =a;
	}

	/// <summary>
	/// Obtains value of gradient used, then reduces the remaining gradient by that much
	/// </summary>
	/// <param name="idx"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	inline double Gradients::ReduceGrad(int idx, int size) {
		double used_amount = grads[idx] - (grads[idx] >= 1) * reduce(size) * std::ceil(step);
		grads[idx] = 1 - used_amount;
		step = stepper(step, size);
		return used_amount;
	}

	inline void Gradients::Reset()
	{
		std::fill(grads.begin(), grads.end(), 1);
		step = 0;
	}
}