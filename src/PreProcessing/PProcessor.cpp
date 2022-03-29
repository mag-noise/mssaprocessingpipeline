#include "PProcessor.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <deque>

namespace PreProcessing {
	using namespace std;
	class PreProcessor {
		template< typename InputValue, typename Container = deque<InputValue>>
		void PreProcessing::PreProcessor::CreateValidSegment(string& input_file, Container const* output_container)
		{

			ifstream file(input_file);
			string line;
			InputValue val;
			if (file.is_open()) {
				while (getline(file, line)) {
					std::stringstream line_stream(line);
					while (line_stream >> val) {
						output_container->push_back(val);
						if (line_stream.peek() == ',') line_stream.ignore();
					}
				}
			}
		}
	};

}