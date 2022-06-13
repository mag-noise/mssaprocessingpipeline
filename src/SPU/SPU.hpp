#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm> // for copy, fill
#include <iterator> // For back_inserter
#include <valarray>
#include "../MSSA/MSSA.hpp"
#ifdef _MAT_
#include "MatlabEngine.hpp"
#include "MatlabDataArray.hpp"
#endif // !_MAT_

// TODO: Add functionality for parallelism
namespace SignalProcessingUnit{
	using namespace std;
	template < typename T , typename A = std::vector<T>>
	class MSSAProcessingUnit {

	private:
		
		bool is_xyz = true;
		bool is_inboard;
		std::map<char, std::vector<A>> _segmented_signal_container;
		std::vector<int> SegmentIndices(A& container, std::function<int(int)> indexer);

		
	public:
		std::map<char, std::function<int(int)>> idx;
		// TODO: update to cleanly check that only container types are used
		MSSAProcessingUnit(bool board) { 
			_segmented_signal_container = map<char, std::vector<A>>();
			idx = map<char, std::function<int(int)>>();
			idx['x'] = [](int a) {return a * 3; };
			idx['y'] = [](int a) {return a * 3 + 1; };
			idx['z'] = [](int a) {return a * 3 + 2; };
			idx['a'] = [](int a) {return a; };
			is_inboard = board;
		}

#ifdef _MAT_
		int LoadFromMatlab(std::u16string input_file);
		int SaveToMatlab();
#endif // !_MAT_

		void LoadCSVData(string& input_file, A& output_container);
		void PreProcess(A data_to_load, bool xyz = false);
		void SetSegmentedValues(char index, double seg_index, T value);
		void BuildSignal(Processor::MSSA::ReconstructionMatrix mat, std::forward_list<int> iarrOfIndices, char mapping, int index);
		void static Process(MSSAProcessingUnit &inboard, MSSAProcessingUnit &outboard, double alpha = 0.005, int num_of_threads = 1);
		A Join();
		A Join(char);
		std::vector<A> operator[](char);
		std::size_t size();
	};

	/// <summary>
	/// 
	/// </summary>
	template<typename T, typename A>
	inline std::vector<int> MSSAProcessingUnit<T, A>::SegmentIndices(A& container, std::function<int(int)> indexer)
	{
		using namespace Processor;
		std::vector<int> indices = {};
		int container_size = std::size(container);
		int num_of_segments = container_size / Processor::MSSA::InputSize() / (indexer(1) / 3 * 2 + 1);
		for (auto i = 0; i < num_of_segments; i++)
			indices.push_back((indexer(1) / 3 * 2 + 1) * i * Processor::MSSA::InputSize());
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
		is_xyz = false;
	}

#ifdef _MAT_
	/// <summary>
	/// Static function to load Matfile data into some container type
	/// </summary>
	/// <typeparam name="T">Type of data contained</typeparam>
	/// <typeparam name="A">Container type for data</typeparam>
	/// <param name="input_file">File Path for CSV data</param>
	/// <param name="output_container">Container for data</param>
	template<typename T, typename A>
	inline int MSSAProcessingUnit<T, A>::LoadFromMatlab(std::u16string file)
	{
		using namespace matlab::engine;

		// Start MATLAB engine synchronously
		std::unique_ptr<MATLABEngine> matlabPtr = startMATLAB();
		
		std::u16string matCommand = u"load(\"";
		matCommand += file + u"\", \"-mat\");";
		// Evaluate MATLAB statement
		matlabPtr->eval(matCommand);
		std::u16string board;

		// Get the result from MATLAB
		if (is_inboard)
			board = u"B_Inboard";
		else
			board = u"B_Outboard";

		
		matlab::data::TypedArray<double> result1 = matlabPtr->getVariable(board);

		// Display results
		std::cout << "Board dimensions: " << result1.getDimensions()[0] << ", " << result1.getDimensions()[1] << std::endl;

		//std::cout << "Board type: " << typeid(&result1[0][0]).name() << std::endl;
		std::cout << "Board first element: " << result1[0][0] << std::endl;

		A dest(result1.begin(), result1.end());
		//std::copy(dest.begin() + (dest.size() / 3 * axes[axis]), (dest.end() * (axes[axis] + 1) / 3), inboard_container);
		PreProcess(dest, true);

		//std::for_each(result1.begin() + (result1.getDimensions()[0] * axes[axis]), (result1.end() * axes[axis] / 3), [result1](double val) {cout << val;});
		//std::for_each(result2.begin() + (result2.getDimensions()[0] * axes[axis]), (result2.end() * axes[axis] / 3), [result2](double val) {cout << val;});


		return 1;
	}

	template<typename T, typename A>
	inline int MSSAProcessingUnit<T, A>::SaveToMatlab() {
		using namespace matlab::engine;

		// Start MATLAB engine synchronously
		std::unique_ptr<MATLABEngine> matlabPtr = startMATLAB();


		//Create MATLAB data array factory
		matlab::data::ArrayFactory factory;

		// Create variables    
		matlab::data::TypedArray<double> data = factory.createArray<double>(_segmented_signal_container['x'], _segmented_signal_container['y'], _segmented_signal_container['z']);


		// Get the result from MATLAB
		if (is_inboard)
			board = u"B_Inboard";
		else
			board = u"B_Outboard";

		// Put variables in the MATLAB workspace
		matlabPtr->setVariable(board, std::move(data));

		return 1;
	}
#endif // !_MAT_

	/// <summary>
	/// Pre-Processing step. Validates and segments data into correct format for matrix operations
	/// </summary>
	/// <typeparam name="T">Data type being worked on</typeparam>
	/// <typeparam name="A">Container type for data</typeparam>
	/// <param name="data_to_load">Concrete container of data to be segmented</param>
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::PreProcess(A data_to_load, bool xyz = false)
	{
		
		// Basic Segmentation (slicing into batches of input_size)
		if (!xyz) {
			std::vector<int> indices = SegmentIndices(data_to_load, idx['a']);
			this->_segmented_signal_container['a'] = std::vector<A>();
			std::for_each(indices.begin(), indices.end(), [this, data_to_load](int i) {
				std::vector<T> copy_to = {};
				std::copy(data_to_load.begin() + i, data_to_load.begin() + i + Processor::MSSA::InputSize(), back_inserter(copy_to));
				_segmented_signal_container['a'].push_back(copy_to);
				});
		}
		else {
			std::vector<int> indices = SegmentIndices(data_to_load, idx['x']);
			for (char val = 'x'; val <= 'z'; val++) {
				this->_segmented_signal_container[val] = std::vector<A>();
				std::for_each(indices.begin(), indices.end(), [this,val, data_to_load](int i) {
					std::vector<T> copy_to = {};
					//std::copy(data_to_load.begin() + i, data_to_load.begin() + i + Processor::MSSA::input_size, back_inserter(copy_to));
					for (auto j = 0; j < Processor::MSSA::InputSize(); j++) {
						copy_to.push_back(data_to_load[i + idx[val](j)]);
					}
					_segmented_signal_container[val].push_back(copy_to);
					});

			
			}
		}
	}
	
	template<typename T, typename A>
	inline void SetSegmentedValues(char index, double seg_index, T value) {
		_segmented_signal_container[index][seg_index] = value;
	}


	template<typename T, typename A>
	inline std::vector<A> MSSAProcessingUnit<T, A>::operator[](char index)
	{
		return _segmented_signal_container[index];
	}


	template<typename T, typename A>
	inline std::size_t MSSAProcessingUnit<T,A>::size() 
	{
		return std::size(_segmented_signal_container.begin()->second);
	}

	/// <summary>
	/// Function to put the reconstruction matrix back into a single signal
	/// </summary>
	/// <param name="mat"></param>
	/// <param name="iarr_of_indices"></param>
	/// <returns></returns>
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::BuildSignal(Processor::MSSA::ReconstructionMatrix mat, std::forward_list<int> iarrOfIndices, char mapping, int index)
	{
		using Processor::MSSA;
		std::fill(_segmented_signal_container[mapping][index].begin(), _segmented_signal_container[mapping][index].end(), 0);
		for (const auto& x : iarrOfIndices) {
			for (auto val = 0; val < MSSA::InputSize(); val++) {
				if ((x < MSSA::WindowSize() * MSSA::number_of_signals && is_inboard) || (x >= MSSA::WindowSize() * MSSA::number_of_signals && !is_inboard)) {
					_segmented_signal_container[mapping][index][val] += mat.col(x)[val];
				}
			}
		}
		

	}

	/// <summary>
	/// Function to send processing units through MSSA
	/// </summary>
	/// <typeparam name="T"></typeparam>
	/// <typeparam name="A"></typeparam>
	/// <param name="num_of_threads"></param>
	/// <param name="inboard"></param>
	/// <param name="outboard"></param>
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T,A>::Process(MSSAProcessingUnit<T,A> &inboard, MSSAProcessingUnit<T,A> &outboard, double alpha=0.005, int num_of_threads = 1) {
		using Processor::MSSA;
		using Eigen::Dense;
		for (char vec = 'x'; vec <= 'z'; vec++) {
			for(int idx = 0; idx < inboard.size(); idx++){
				MSSA::ReconstructionMatrix mat = MSSA::Process(inboard[vec][idx], outboard[vec][idx]);

#ifdef _DEBUG
				if (vec == 'x' && idx == 0) {
					// TODO: Reconstruct original vectors
					string path = "eigenvector.csv";
					ofstream ofs(path);

					// CSV Formatting example: https://stackoverflow.com/questions/61987600/write-eigen-vectorxd-in-csv-format
					using namespace Eigen;
					IOFormat OctaveFmt(StreamPrecision, 0, ", ", "\n", "", "", "", "");
					ofs << (mat).format(OctaveFmt);
					ofs.close();
				}
				MSSA::ValidSignal inboardOriginal = inboard[vec][idx];
#endif
				auto componentList = MSSA::ComponentSelection(mat, inboard[vec][idx], outboard[vec][idx], alpha);
				inboard.BuildSignal(mat, componentList, vec, idx);
				outboard.BuildSignal(mat, componentList, vec, idx);
#ifdef _DEBUG
				//MSSA::ValidSignal inboardRecon = inboard[vec][idx];
				//MSSA::ValidSignal outboardRecon = outboard[vec][idx];

				//std::cout << "Mat " << idx << " size: " << mat.size() << std::endl;
				//std::cout << "N rows: " << mat.rows() << std::endl;
				//std::cout << "Row 1: " << mat.row(0) << std::endl;
				//std::cout << "N cols: " << mat.cols() << std::endl;



				//std::cout << "Original Signal: ";
				//for_each(inboardOriginal.begin(), inboardOriginal.end(), [](double a) {std::cout << a << ", "; });
				//std::cout << std::endl;

				//std::cout << "Reconstructed Signal: ";
				//for_each(inboardRecon.begin(), inboardRecon.end(), [](double a) {std::cout << a << ", "; });
				//std::cout << std::endl;
				break;
#endif // _DEBUG
			}
		}
	}

	template<typename T, typename A>
	inline A MSSAProcessingUnit<T, A>::Join() {
		A output = A();
		// Interesting solution to efficient concatenation of vectors:
		// https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors
		/*output.reserve(_segmented_signal_container['x'].size() * Processor::MSSA::input_size * 3);
		auto lam = [&output](A segment) {
			output.insert(output.end(), segment.begin(), segment.end());
		};
		for (auto idx = 'x'; idx <= 'z'; idx++) {
			std::for_each(_segmented_signal_container[idx].begin(), _segmented_signal_container[idx].end(), lam);
		}*/

		for (auto i = 0; i < _segmented_signal_container['x'].size(); i++) {
			for (auto j = 0; j < _segmented_signal_container['x'][0].size(); j++) {
				for (auto idx = 'x'; idx <= 'z'; idx++) {
					output.push_back(_segmented_signal_container[idx][i][j]);
				}
			}
		}
		return output;
	}

	template<typename T, typename A>
	inline A MSSAProcessingUnit<T, A>::Join(char idx) {
		A output;
		output.reserve(_segmented_signal_container[idx].size() * Processor::MSSA::InputSize());
		std::for_each(_segmented_signal_container[idx].begin(), _segmented_signal_container[idx].end(), [&output](A segment) {
			output.insert(output.end(), segment.begin(), segment.end());
			});
		return output;
	}
	


}