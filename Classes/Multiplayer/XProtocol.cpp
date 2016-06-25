#include "XProtocol.hpp"
#include <cstdlib>
#include <sstream>
#include <bitset>
using std::stringstream;

// Implementation for all the conversion methods
template <>
std::string XProtocol::convert<std::string, std::string>(std::string myItem) {
	return myItem;
};

template <>
std::string XProtocol::convert<std::string, signed int>(signed int myInt) {
	stringstream ss;
	ss << myInt;
	return ss.str();
};

template <>
int XProtocol::convert<signed int, std::string>(std::string myString) {
	return std::atol(myString.c_str());
};

template <>
std::string XProtocol::convert<std::string, char>(char myChar) {
	return convert<std::string, int>(myChar);
}

template <>
char XProtocol::convert<char, std::string>(std::string myString) {
	return convert<int, std::string>(myString);
}

template <>
std::string XProtocol::convert<std::string, unsigned char>(unsigned char myChar) {
	return convert<std::string, int>(myChar);
}

template <>
unsigned char XProtocol::convert<unsigned char, std::string>(std::string myString) {
	return convert<int, std::string>(myString);
}

template <>
std::string XProtocol::convert<std::string, unsigned int>(unsigned int myChar) {
	stringstream ss;
	ss << myChar;
	return ss.str();
}

template <>
unsigned int XProtocol::convert<unsigned int, std::string>(std::string myString) {
	return std::atol(myString.c_str());
}

template <>
std::string XProtocol::convert<std::string, float>(float myFloat) {
	std::stringstream ss;
	ss << myFloat;
	return ss.str();
}

template <>
float XProtocol::convert<float, std::string>(std::string myString) {
	float re;
	std::stringstream ss(myString);
	ss >> re;
	return re;
}

template <>
std::string XProtocol::convert<std::string, double>(double myDouble) {
	std::stringstream ss;
	ss << myDouble;
	return ss.str();
}

template <>
double XProtocol::convert<double, std::string>(std::string myString) {
	double re;
	std::stringstream ss(myString);
	ss >> re;
	return re;
}

template <>
std::string XProtocol::convert<std::string, Vec2i>(Vec2i myVec) {
	std::stringstream ss;
	ss << myVec.x << " " << myVec.y;
	return ss.str();
}

template <>
Vec2i XProtocol::convert<Vec2i, std::string>(std::string myString) {
	Vec2i re;
	std::stringstream ss (myString);
	ss >> re.x;
	ss >> re.y;
	return re;
}