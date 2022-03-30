#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>


namespace PreProcessing{
	using namespace std;
	template < typename T >
	class MSSAPreProcessor {
		public:
			// TODO: update to cleanly check that only container types are used
			void static CreateValidSegment(string& input_file, vector<T>& output_container);
	};

	template<class InputType>
	void MSSAPreProcessor<InputType>::CreateValidSegment(string& input_file, vector<InputType>& output_container)
	{
		ifstream file{ input_file };
		string line;
		InputType val;
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