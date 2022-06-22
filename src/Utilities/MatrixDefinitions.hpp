#pragma once
#include <algorithm>
#include <vector>
namespace Utils{
	class FlagSystem
	{
    public:
        struct flag {
        public:
            uint8_t  is_nan : 1, time_jump:1;
            const bool flag::FlagRaised() {
                return (bool)(is_nan + time_jump);
            }
            friend bool operator<(flag& lhs, flag& rhs) { 
                return lhs.FlagRaised() && rhs.FlagRaised();
            }
            friend bool operator>(flag& lhs, flag& rhs) { return rhs < lhs; }
        };
	private:
        static FlagSystem* instance;
        FlagSystem() {};
        std::vector<flag> flags;
    public:
        static FlagSystem* GetInstance() {
            if (!instance)
                instance = new FlagSystem();
            return instance;
        }

        std::size_t Size() {
            return instance->flags.size();
        }

        void Resize(std::size_t i) {
            instance->flags.resize(i);
        }
        
        /// <summary>
        /// Checks if there is a flag raised in the segment. If there is, the next segment will start after the last flag. 
        /// If there isn't, the next segment will start at the end of the current one
        /// </summary>
        /// <param name="start_idx"></param>
        /// <param name="segment_size"></param>
        /// <returns></returns>
        std::size_t FindFlagInSegment(std::size_t start_idx, std::size_t segment_size) {
            std::vector<flag> unraised = { flag() };
            // Find the last flag from the starting index to the valid segment size.
            auto x =std::find_end(instance->flags.begin() + start_idx, instance->flags.begin() + start_idx + segment_size, unraised.begin(), unraised.end(), operator>);
            
            // Convert iterator to integer index
            return int(x - (instance->flags.begin()));
        }

        flag& operator[] (std::size_t i) {
            return instance->flags[i];
        }

	};
}