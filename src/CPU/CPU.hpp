#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include "../MSSA/MSSA.hpp"

// TODO: Add functionality for parallelism
namespace CentralProcessingUnit{
	using namespace std;
	template < typename T , typename A = std::vector<T>>
	class MSSAProcessingUnit {

	private:
		std::vector<A> _segmented_signal_container{};
		std::vector<int> SegmentIndices(A& container);
	public:
		// TODO: update to cleanly check that only container types are used
		MSSAProcessingUnit() { _segmented_signal_container = new vector<A>; }

		void static LoadCSVData(string& input_file, A& output_container);
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