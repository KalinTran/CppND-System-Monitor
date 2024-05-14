#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {    
    long min = seconds / 60;
    long hour = min / 60;
    std::string hh = std::to_string(hour);
    std::string mm = std::to_string(min % 60);
    std::string ss = std::to_string(seconds % 60);
    std::string time = hh + ":" + mm + ":" + ss + "  ";  
    return time; 
}