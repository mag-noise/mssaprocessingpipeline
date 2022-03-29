#pragma once
#include <string>
#include <fstream>
namespace PreProcessing{
	using namespace std;
	class PreProcessor{
		public:
			// TODO: update to cleanly check that only container types are used
			template<typename Container>
			void static CreateValidSegment(string&, Container const *);
	};
	
}