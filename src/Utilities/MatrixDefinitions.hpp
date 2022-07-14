#pragma once
#include <algorithm>
#include <vector>
#include <numeric>
#include <cmath>
#include <stdexcept>
#ifdef _DEBUG
#include <assert.h>
#endif // !



namespace Utils{
	/// <summary>
	/// Singleton class to maintain flags across signals
	/// </summary>
	class FlagSystem
	{
    public:
        enum flagtype {
            nan, t_jump, skipped, merge
        };

        /// <summary>
        /// Inner structure to capture what flags can be raised and how to handle them
        /// </summary>
        struct flag {
        public:
            uint8_t  is_nan : 1, time_jump:1, skipped_value:1, merge_required:1, time_jump_used:1;

            const bool FlagRaised() {
                return (bool)(is_nan || (time_jump&&!time_jump_used) || skipped_value);
            }

            const bool TimeJumpOnly() {
                return (bool)time_jump && !(is_nan || time_jump_used);
            }
            
            // Current makeup of flags:
            // 0 0 0 0 Merge Skipped T_Jump NaN
            // POTENTIAL EXTENSIONS: Inf values | Eigenvector unable to be calculated
            operator int() const { return(uint8_t)((is_nan << nan) | (time_jump << t_jump) | (skipped_value << skipped) | (merge_required << merge)); }

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
        /// <summary>
        /// Function to access instance of singleton
        /// </summary>
        static FlagSystem* GetInstance() {
            if (!instance)
                instance = new FlagSystem();
            return instance;
        }

        /// <summary>
        /// Size of singleton
        /// </summary>
        std::size_t Size() {
            return instance->flags.size();
        }

        /// <summary>
        /// Allows changing the size of the singleton
        /// </summary>
        void Resize(std::size_t i) {
            instance->flags.resize(i);
        }

        /// <summary>
        /// Resets the state of all flags in the singleton
        /// </summary>
        /// <typeparam name="A"></typeparam>
        /// <param name="container"></param>
        void Reset() {
            auto currSize = Size();
            instance->flags.clear();
            Resize(currSize);
        }


        /// <summary>
        /// Provides an integer conversion of the flags within the singleton
        /// </summary>
        std::vector<int> Snapshot() {
            return std::vector<int>(instance->flags.begin(), instance->flags.end());
        }

        /// <summary>
        /// Function to flag NaN values within the provided container.
        /// </summary>
        /// <typeparam name="A">Indexable container type. Requires implementation of the size() function.</typeparam>
        /// <param name="container"></param>
        template<typename A = std::vector<double>>
        void FlagNaN(A container) {
            if (Size() == 0)
                Resize(container.size());

            int i = 0;
            for (i; i < container.size(); i++) {
                instance->flags[i].is_nan |= std::isnan(container[i]);
            }

        }

        /// <summary>
        /// Function to setup timeseries flags
        /// </summary>
        /// <param name="timeseries"></param>
        void FlagDiscontinuity(std::vector<double> timeseries) {
            if (Size() == 0)
                Resize(timeseries.size());

            //std::vector<double> timediff = std::vector<double>(timeseries.size());
            std::adjacent_difference(timeseries.begin(), timeseries.end(), timeseries.begin());
            double sum = std::accumulate(timeseries.begin(), timeseries.end(), 0.0);
            double mean = sum / timeseries.size();

            for (auto i = 1; i < timeseries.size(); i++)
                instance->flags[i].time_jump |= (timeseries[i]) > mean;
        }
        
        /// <summary>
        /// Function to flag a segment as skipped
        /// </summary>
        /// <param name="start"></param>
        /// <param name="segment_size"></param>
        void FlagSegment(int start, int segment_size) {
            if (start + segment_size <= Size())
                throw std::invalid_argument("Invalid segment constraints. Unable to flag the full requested segment");
            std::for_each(instance->flags.begin() + start, instance->flags.begin() + start + segment_size, [](flag& val) {
                val.skipped_value |= 1; 
                });
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
            if(start_idx >= Size() || (!valid_past && (start_idx + segment_size) > Size()) || segment_size <= 0)
                return;

            // Logic for last segment not fitting size requirements
            if (valid_past && (start_idx + segment_size) > Size()) {
                idx_vector.push_back(Size() - segment_size);

                // Flag merge needed for elements in the last segment
                std::for_each(instance->flags.begin() + Size() - segment_size, instance->flags.begin() + start_idx, [](flag& ele) {ele.merge_required |= 1; });
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

                // +1 accounts for moving the flagged value past the flagged instance. However, if the flag is a time jump, we are allowed to worked on it as a starting point
                int y = int(last - (instance->flags.begin())+1*(!instance->flags[last - (instance->flags.begin())].TimeJumpOnly()));
                if (y == last - (instance->flags.begin()))
                {
                    instance->flags[y].time_jump_used |= 1;
                    FindFlagInSegment(y, segment_size, idx_vector, false);
                    return;
                }

                
                // If availabe space before starting index, utilize values prior to the first flagged instance
                if (valid_past) {
                    idx_vector.push_back(int(first - (instance->flags.begin())) - segment_size);
                    std::for_each(first - segment_size, instance->flags.begin()+start_idx, [](flag& ele) {ele.merge_required |= 1; });
                }
                else {
                    std::for_each(instance->flags.begin() + start_idx, first+1, [](flag& ele) { ele.skipped_value |= 1; });
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
        
        /// <summary>
        /// Looks for any merge requirements in a segment.
        /// </summary>
        /// <param name=""></param>
        /// <param name="segment_size"></param>
        void GetMergesInSegment(std::size_t start_idx, std::size_t segment_size, std::pair<int, int>& idx_pair) {
            if (instance->flags[start_idx + segment_size - 1].merge_required == 0) {
                idx_pair.first = -1;
                idx_pair.second = -1;
                return;
            }

            int i = start_idx + segment_size - 1;

            idx_pair.first = i;
            while (i >= 0 && instance->flags[i].merge_required) {
                i--;
            }
            idx_pair.second = i + (i < start_idx);
        }

        /// <summary>
        /// Access to individual flags
        /// </summary>
        flag& operator[] (std::size_t i) {
            return instance->flags[i];
        }

	};
}