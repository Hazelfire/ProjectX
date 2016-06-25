#pragma once
#include <string>
class StringOps {
public:
	static std::string trim(std::string);
	static std::string rtrim(std::string);
	static std::string ltrim(std::string);
	static std::string to_string(int value);
};