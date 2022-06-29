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
#include <numeric>
#include "../MSSA/MSSA.hpp"
#include "../Utilities/MatrixDefinitions.hpp"
#include "../Utilities/Gradients.hpp"
#ifdef _MAT_
#include "MatlabEngine.hpp"
#include "MatlabDataArray.hpp"
#endif // !_MAT_

// Using SPU instanciates flagsystem instance linking

// TODO: Add functionality for parallelism
namespace SignalProcessingUnit{
	using namespace std;
	template < typename T , typename A = std::vector<T>>
	class MSSAProcessingUnit {
	private:

		bool is_xyz = true;
		bool is_inboard;
		std::map<char, std::vector<A>> _segmented_signal_container;
		std::map<char, std::vector<A>> _wheel_container;
		std::vector<int> _indices;
		Utils::FlagSystem *flags = Utils::FlagSystem::GetInstance();
		void SegmentIndices(A& container, std::function<int(int)> indexer);
		void SetupContainer(A data_to_load, pair<char, char> start_end, std::map<char, std::vector<A>>& container);
		A JoinContainer(A original, std::map<char, std::vector<A>>&);
		A ContainerOperation(std::map<char, std::vector<A>>& container);
		void BuildSignalContainer(Processor::MSSA::ReconstructionMatrix mat, std::vector<int> iarrOfIndices, char mapping, int index, std::map<char, std::vector<A>> &container);

	public:

		std::map<char, std::function<int(int)>> idx;
		// TODO: update to cleanly check that only container types are used
		MSSAProcessingUnit(bool board) { 
			_segmented_signal_container = map<char, std::vector<A>>();
			_wheel_container = map<char, std::vector<A>>();

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
		void BuildSignal(Processor::MSSA::ReconstructionMatrix mat, std::vector<int> iarrOfIndices, char mapping, int index);
		void static Process(MSSAProcessingUnit &inboard, MSSAProcessingUnit &outboard, double alpha = 0.05, int num_of_threads = 1);
		A JoinSignal(A original = A());
		A JoinWheel();
		std::vector<A> operator[](char);
		std::size_t size();
	};

	using SignalProcessingUnit::MSSAProcessingUnit;


#pragma region private
	/// <summary>
	/// Function to obtain the starting indices of each segment
	/// </summary>
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::SegmentIndices(A& container, std::function<int(int)> indexer)
	{
		using namespace Processor;
		//int container_size = std::size(container);
		//int max_num_of_segments = container_size / Processor::MSSA::InputSize() / (indexer(1) / 3 * 2 + 1);
		// How to segment:
		// 1. Segment should be segment_size
		// 2. A segment should not contain any discontinuity
		
		// TODO: Extend flagging to look backwards for a potentially valid segment. Remake function call to be recursive?

		// Creates the index for the starting point of each segment
		flags->FindFlagInSegment(0, Processor::MSSA::InputSize() * (indexer(1) / 3 * 2 + 1), _indices);

	}


	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::SetupContainer(A data_to_load, pair<char, char> start_end, std::map<char, std::vector<A>>& container) {
		for (char val = start_end.first; val <= start_end.second; val++) {
			container[val] = std::vector<A>();
			std::for_each(_indices.begin(), _indices.end(), [this, &container, val, data_to_load](int i) {
				std::vector<T> copy_to = {};
				for (auto j = 0; j < Processor::MSSA::InputSize(); j++) {
					copy_to.push_back(data_to_load[i + idx[val](j)]);
				}
				container[val].push_back(copy_to);
				});


		}
	}


	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::BuildSignalContainer(Processor::MSSA::ReconstructionMatrix mat,
		std::vector<int> iarrOfIndices, char mapping, int index, std::map<char, std::vector<A>>& container) {
		using Processor::MSSA;

		std::fill(container[mapping][index].begin(), container[mapping][index].end(), 0);
		for (const auto& x : iarrOfIndices) {
			for (auto val = 0; val < MSSA::InputSize(); val++) {
				if ((x < MSSA::WindowSize() * MSSA::number_of_signals && is_inboard) || (x >= MSSA::WindowSize() * MSSA::number_of_signals && !is_inboard)) {
					container[mapping][index][val] += mat.col(x)[val];
				}
			}
		}
	}


	template<typename T, typename A>
	inline A MSSAProcessingUnit<T, A>::JoinContainer(A original, std::map<char, std::vector<A>>& container) {
		// INITIALIZE TO ORIGINAL VALUES
		if (original.size() == 0)
			original = A(flags->Size(), nan("-ind"));
		// Interesting solution to efficient concatenation of vectors:
		// https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors
		/*output.reserve(_segmented_signal_container['x'].size() * Processor::MSSA::input_size * 3);
		auto lam = [&output](A segment) {
			output.insert(output.end(), segment.begin(), segment.end());
		};
		for (auto idx = 'x'; idx <= 'z'; idx++) {
			std::for_each(_segmented_signal_container[idx].begin(), _segmented_signal_container[idx].end(), lam);
		}*/

		A reconstruction = ContainerOperation(container);

		for (int i = 0; i < original.size(); i++) {
			if (!(*flags)[i].FlagRaised()) {
				original[i] = (reconstruction[i] * !(*flags)[i].FlagRaised());
			}
		}
		return original;
	}

	template<typename T, typename A>
	inline A MSSAProcessingUnit<T, A>::ContainerOperation(std::map<char, std::vector<A>>& container) {
		// INITIALIZE TO ORIGINAL VALUES
		A original = A(flags->Size(), 0);
		// Interesting solution to efficient concatenation of vectors:
		// https://stackoverflow.com/questions/3177241/what-is-the-best-way-to-concatenate-two-vectors
		/*output.reserve(_segmented_signal_container['x'].size() * Processor::MSSA::input_size * 3);
		auto lam = [&output](A segment) {
			output.insert(output.end(), segment.begin(), segment.end());
		};
		for (auto idx = 'x'; idx <= 'z'; idx++) {
			std::for_each(_segmented_signal_container[idx].begin(), _segmented_signal_container[idx].end(), lam);
		}*/

		std::pair<int, int> start_end(-1, -1);
		Utils::Gradients gradients(original.size());

		// Static: Sets to use linear reduce
		gradients.SetReduce([](int i) -> double { 
			double reduced_value = std::abs(1.0 / (double(i - 1) + !(i - 1 <= 0)));
			if (isinf(reduced_value) || isnan(reduced_value))
				return 1.0;
			return reduced_value;
			});

		// Stepper set to use correct equation based on xyz or not. Stepper resets to 0 after passing size;
		gradients.SetStepper([this](double step, int size) -> double {
			double stepping = (step + 1.0 / 3.0 * is_xyz + !is_xyz);
			return double(!(stepping > size) * stepping);
			});

		// Go through all values and assign gradient
		for (auto i = 0; i < container['x' * is_xyz + 'a' * !is_xyz].size(); i++) {
			flags->GetMergesInSegment(_indices[i], Processor::MSSA::InputSize(), start_end);
			for (auto j = 0; j < container['x' * is_xyz + 'a' * !is_xyz][0].size(); j++) {
				for (auto indx = 'x' * is_xyz + 'a' * !is_xyz; indx <= ('z' * is_xyz + 'a' * !is_xyz); indx++) {
					int pos = _indices[i] + idx[indx](j);
					original[pos] = original[pos] + std::move(container[indx][i][j]) * gradients.ReduceGrad(pos, start_end.second-start_end.first);
				}
			}
		}
		return original;
	}
#pragma endregion

#pragma region loading
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
		T val = T();
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

		// Generate flag vector
		flags->Resize(output_container.size());
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
		
		// Generate flag vector
		flags->Resize(dest.size());
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


#pragma endregion


	/// <summary>
	/// Pre-Processing step. Validates and segments data into correct format for matrix operations
	/// </summary>
	/// <typeparam name="T">Data type being worked on</typeparam>
	/// <typeparam name="A">Container type for data</typeparam>
	/// <param name="data_to_load">Concrete container of data to be segmented</param>
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::PreProcess(A data_to_load, bool xyz)
	{
		if (flags->Size() == 0)
			flags->Resize(data_to_load.size());

		// Obtain all segment indices
		SegmentIndices(data_to_load, idx['a'*!xyz + 'x'*xyz]);

		// Setup containers to have original values
		pair<char, char> start_end('a' * !xyz + 'x' * xyz, 'a' * !xyz + 'z' * xyz);
		SetupContainer(data_to_load, start_end, _segmented_signal_container);
		SetupContainer(data_to_load, start_end, _wheel_container);
	}

	
	template<typename T, typename A>
	inline void MSSAProcessingUnit<T,A>::SetSegmentedValues(char index, double seg_index, T value) {
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
	inline void MSSAProcessingUnit<T, A>::BuildSignal(Processor::MSSA::ReconstructionMatrix mat, std::vector<int> iarrOfIndices, char mapping, int index)
	{
		// Build main signal
		BuildSignalContainer(mat, iarrOfIndices, mapping, index, _segmented_signal_container);
		
		// Build Wheel (error signal)
		std::vector<int> allIndices(mat.cols());
		int i = 0;
		std::sort(iarrOfIndices.begin(), iarrOfIndices.end());
		std::generate(allIndices.begin(), allIndices.end(), [&i]() {return i++; });
		std::vector<int> diff;
		std::set_difference(allIndices.begin(), allIndices.end(), iarrOfIndices.begin(), iarrOfIndices.end(),
			std::back_inserter(diff));

		BuildSignalContainer(mat, diff, mapping, index, _wheel_container);

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
	inline void MSSAProcessingUnit<T,A>::Process(MSSAProcessingUnit<T,A> &inboard, MSSAProcessingUnit<T,A> &outboard, double alpha, int num_of_threads) {
		using Processor::MSSA;
		using Eigen::Dense;
		for (char vec = 'x'; vec <= 'z'; vec++) {
			for(int idx = 0; idx < inboard.size(); idx++){
				MSSA::ReconstructionMatrix mat = MSSA::Process(inboard[vec][idx], outboard[vec][idx]);

#ifdef _TEST
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
#ifdef _TEST
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
	inline A MSSAProcessingUnit<T, A>::JoinSignal(A original) {
		return JoinContainer(original, _segmented_signal_container);
	}

	template<typename T, typename A>
	inline A MSSAProcessingUnit<T, A>::JoinWheel() {
		return JoinContainer(A(0), _wheel_container);
	}

	


}