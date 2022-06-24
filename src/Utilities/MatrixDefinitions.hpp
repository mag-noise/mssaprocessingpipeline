#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
namespace Utils{
	class FlagSystem
	{
    public:
        enum flagtype {
            nan, t_jump, skipped, merge
        };

        struct flag {
        public:
            uint8_t  is_nan : 1, time_jump:1, skipped_value:1, merge_required:1;
            const bool flag::FlagRaised() {
                return (bool)(is_nan || time_jump || skipped_value);
            }
            const uint8_t Build() {
                return (uint8_t)((is_nan << nan) | (time_jump << t_jump) | (skipped_value << skipped) | (merge_required << merge));
            }

            friend bool operator<(flag& lhs, flag& rhs) { 
                return !lhs.FlagRaised() && rhs.FlagRaised();
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

        void Reset() {
            auto currSize = Size();
            instance->flags.clear();
            Resize(currSize);
        }

        template<typename A = std::vector<double>>
        void FindNaN(A container) {
            if (Size() == 0)
                Resize(container.size());
            std::vector<int> nan_ind = std::vector<int>();
            for (auto i = 0; i < container.size(); i++) {
                instance->flags[i].is_nan = std::isnan(container[i]);

            }
        }

        /// <summary>
        /// Function to setup timeseries flags
        /// </summary>
        /// <param name="timeseries"></param>
        void FlagDiscontiunity(std::vector<double> timeseries) {
            if (Size() == 0)
                Resize(timeseries.size());

            double sum = std::accumulate(timeseries.begin(), timeseries.end(), 0.0);
            double mean = sum / timeseries.size();

            double sq_sum = std::inner_product(timeseries.begin(), timeseries.end(), timeseries.begin(), 0.0);
            double stdev = std::sqrt(sq_sum / timeseries.size() - mean * mean);

            for (auto i = 1; i < timeseries.size(); i++)
                instance->flags[i].time_jump = (timeseries[i] - timeseries[i - 1]) > stdev;
        }
        
        /// <summary>
        /// Checks if there is a flag raised in the segment. If there is, the next segment will start after the last flag. 
        /// If there isn't, the next segment will start at the end of the current one
        /// TODO: Finish adding logic for marking merge_required and skipped values
        /// </summary>
        /// <param name="start_idx"></param>
        /// <param name="segment_size"></param>
        /// <returns></returns>
        void FindFlagInSegment(std::size_t start_idx, std::size_t segment_size, std::vector<int>& idx_vector, bool valid_past=false) {
            // Logic for recurrive overflow
            if(start_idx >= instance->flags.size())
                return;

            // Logic for last segment not fitting size requirements
            if (valid_past && (start_idx + segment_size) > Size()) {
                idx_vector.push_back(Size() - segment_size);
                
                // Flag merge needed for elements in the last segment
                std::for_each(instance->flags.begin() + Size() - segment_size, instance->flags.begin() + start_idx, [](flag& ele) {ele.merge_required = 1; });
                return;
            }


            std::vector<flag> unraised = { flag() };
            
            // Finds first flag or gives last iterator
            auto first = std::find_if(instance->flags.begin() + start_idx, instance->flags.begin() + start_idx + segment_size, [](flag& instance_flag) {
                flag empty = flag();
                return instance_flag > empty;
                });
            if (first < instance->flags.begin() + start_idx + segment_size) {
                // Find the last flag from the starting index to the valid segment size or gives last iterator
                auto last = std::find_end(instance->flags.begin() + start_idx, instance->flags.begin() + start_idx + segment_size, unraised.begin(), unraised.end(),
                    [](flag& lhs, flag& rhs) {return lhs > rhs; });
                // Flag each element in between first and last instance of invalid values
                std::for_each(first, last+1, [](flag& ele) { ele.skipped_value = 1; });
                int y = int(last - (instance->flags.begin())+1);
                
                // If availabe space before starting index, utilize values prior to the first flagged instance
                if (valid_past) {
                    idx_vector.push_back(int(first - (instance->flags.begin())) - segment_size);
                    std::for_each(first - segment_size, instance->flags.begin()+start_idx, [](flag& ele) {ele.merge_required = 1; });
                }
                else {
                    std::for_each(instance->flags.begin() + start_idx, first+1, [](flag& ele) { ele.skipped_value = 1; });
                }
                FindFlagInSegment(y, segment_size, idx_vector, false);
                //bool pick_idx = (y - start_idx == segment_size);
                //// Convert iterator to integer index
                //idx_vector.push_back((pick_idx)*start_idx + !(pick_idx) * (y + 1));
                
            }
            else
            {
                idx_vector.push_back(start_idx);
                FindFlagInSegment(start_idx + segment_size, segment_size, idx_vector, true);
            }
        }

        flag& operator[] (std::size_t i) {
            return instance->flags[i];
        }

	};
}