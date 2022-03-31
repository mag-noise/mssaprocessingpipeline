#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>


namespace CentralProcessingUnit{
	using namespace std;
	template < typename T , typename A = std::vector<T>>
	class MSSAProcessingUnit {

	public:
		// TODO: update to cleanly check that only container types are used
		void static LoadCSVData(string& input_file, A& output_container);
	};

	template<typename T, typename A>
	inline void MSSAProcessingUnit<T, A>::LoadCSVData(string& input_file, A& output_container)
	{
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


}