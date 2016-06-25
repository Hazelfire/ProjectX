#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include "StringOperations.h"
#include <sstream>
// trim from start
std::string StringOps::ltrim(std::string s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
std::string StringOps::rtrim(std::string s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
std::string StringOps::trim(std::string s) {
        return ltrim(rtrim(s));
}

// convert integer to string
std::string StringOps::to_string(int value) {
	std::stringstream ss;
	ss << value;
	return ss.str();
}