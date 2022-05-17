#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include "../MSSA/MSSA.hpp"
#include "MatlabEngine.hpp"
#include "MatlabDataArray.hpp"


// TODO: Add functionality for parallelism
namespace CentralProcessingUnit{
	using namespace std;
	template < typename T , typename A = std::vector<T>>
	class MSSAProcessingUnit {

	private:
		std::vector<A> _segmented_signal_container{};
		std::vector<int> SegmentIndices(A& container);

		
	public:
		int X_idx = [](int a) {return a; };
		int Y_idx = [](int a) {return a+1; };
		int Z_idx = [](int a) {return a+2; };
		// TODO: update to cleanly check that only container types are used
		MSSAProcessingUnit() { _segmented_signal_container = std::vector<A>(); }

		void static LoadCSVData(string& input_file, A& output_container);
		int static LoadFromMatlab(std::u16string input_file, A& inboard_container, A& outboard_container, char axis='x');
		void PreProcess(A data_to_load);
		void Process(int num_of_threads = 1);
		A& operator[](int);
	};

	template<typename T, typename A>
	inline std::vector<int> MSSAProcessingUnit<T, A>::SegmentIndices(A& container)
	{
		using namespace Processor;
		std::vector<int> indices = {};
		int container_size = sizeof(container) / sizeof(container[0]);
		int num_of_indices = container_size / Processor::MSSA::input_size;
		for (auto i = 0; i < num_of_indices; i++)
			indices.push_back(i * Processor::MSSA::input_size);
		return indices;
	}

	/// <summary>
	/// Static function to load CSV data into some container type
	/// </summary>
	/// <typeparam name="T">Type of data contained</typeparam>
	/// <typeparam name="A">Container type for data</typeparam>
	/// <param name="input_file">File Path for CSV data</param>
	/// <param name="output_container">Container for data</param>
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::LoadCSVData(string& input_file, A& output_container)
	{
		// TODO: Add check that type {A} can use the push_back function and that val is a primitive
		ifstream file{ input_file };
		string line;
		T val;
		if (file.is_open()) {
			while (getline(file, line)) {
				std::stringstream line_stream(line);
				while (line_stream >> val) {
					output_container.push_back(val);
					if (line_stream.peek() == ',') line_stream.ignore();
				}
			}
		}
	}

	/// <summary>
	/// Static function to load Matfile data into some container type
	/// </summary>
	/// <typeparam name="T">Type of data contained</typeparam>
	/// <typeparam name="A">Container type for data</typeparam>
	/// <param name="input_file">File Path for CSV data</param>
	/// <param name="output_container">Container for data</param>
	template<typename T, typename A>
	inline int MSSAProcessingUnit<T, A>::LoadFromMatlab(std::u16string file, A& inboard_container, A& outboard_container, char axis='x')
	{
		using namespace matlab::engine;
		std::map<char, int> axes;
		axes['x'] = 0;
		axes['y'] = 1;
		axes['z'] = 2;
		// Start MATLAB engine synchronously
		std::unique_ptr<MATLABEngine> matlabPtr = startMATLAB();
		
		std::u16string matCommand = u"load(\"";
		matCommand += file + u"\");";
		// Evaluate MATLAB statement
		matlabPtr->eval(matCommand);

		// Get the result from MATLAB
		matlab::data::TypedArray<double> result1 = matlabPtr->getVariable(u"B_Inboard_2RPM");
		matlab::data::TypedArray<double> result2 = matlabPtr->getVariable(u"B_Outboard_2RPM");
		
		// Display results
		std::cout << "B_Inboard_2RPM: " << result1.getDimensions()[0] << ", " << result1.getDimensions()[1] << std::endl;
		std::cout << "B_Outboard_2RPM: " << result2.getDimensions()[0] << ", " << result2.getDimensions()[1] << std::endl;

		std::cout << "B_Inboard_2RPM: " << typeid(&result1[0][0]).name() << std::endl;
		std::cout << "B_Outboard_2RPM: " << result2[0][0] << std::endl;

		A dest(result1.begin(), result1.end());
		//std::copy(dest.begin() + (dest.size() / 3 * axes[axis]), (dest.end() * (axes[axis] + 1) / 3), inboard_container);
		inboard_container = dest;

		A des2(result2.begin(), result2.end());
		//std::copy(dest.begin() + (dest.size() / 3 * axes[axis]), (dest.end() * (axes[axis] + 1) / 3), outboard_container);
		outboard_container = des2;
		std::cout << "Inboard: " << inboard_container.size() << endl;
		//std::for_each(result1.begin() + (result1.getDimensions()[0] * axes[axis]), (result1.end() * axes[axis] / 3), [result1](double val) {cout << val;});
		//std::for_each(result2.begin() + (result2.getDimensions()[0] * axes[axis]), (result2.end() * axes[axis] / 3), [result2](double val) {cout << val;});


		return 1;
	}



	/// <summary>
	/// Pre-Processing step. Validates and segments data into correct format for matrix operations
	/// </summary>
	/// <typeparam name="T">Data type being worked on</typeparam>
	/// <typeparam name="A">Container type for data</typeparam>
	/// <param name="data_to_load">Concrete container of data to be segmented</param>
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::PreProcess(A data_to_load)
	{
		// Basic Segmentation (slicing into batches of input_size)
		std::vector<int> indices = this.SegmentIndices(data_to_load);
		std::for_each(indices.begin(), indices.end(), [indices](&int i) {
				vector<A> copy_to = {};
				std::copy(indices.begin() + i, indices.begin() + i + Processor::MSSA::input_size, copy_to);
				_segmented_signal_container.push_back(copy_to);
			});
		
	}


	template<typename T, typename A>
	inline A& MSSAProcessingUnit<T, A>::operator[](int index)
	{
		return _segmented_signal_container[index];
	}

	


}