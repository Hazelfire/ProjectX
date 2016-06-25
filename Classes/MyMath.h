#pragma once
#include "../cocos2d/cocos/math/Vec2.h"

#define PI 3.141592653


#define ABS(number) number > 0? number: number * -1
template <typename ModType, typename ModType2>
int myMod(ModType a, ModType2 b)
{
	if (b < 0) //you can check for b == 0 separately and do what you want
		return myMod(-a, -b);
	int ret = a % b;
	if (ret < 0)
		ret += b;
	return ret;
}

// This is really cool. I got annoyed using floats for Vec2's when I did not need them so I can now use this. basically it allows
// you to make a Vector of any type (eg, ints, chars and the like)
template <typename Type> struct Vec2X {
	Type x;
	Type y;
	Vec2X<Type>() { x = 0; y = 0; };
	Vec2X<Type>(cocos2d::Vec2 cocoVector) { x = (Type)cocoVector.x; y = (Type)cocoVector.y; };
	Vec2X<Type>(Type px, Type py) { x = px; y = py; };
	template <typename NewType>
	Vec2X<Type>(Vec2X<NewType> other) {
		x = (Type)other.x; y = (Type)other.y;
	};

	bool equals(Vec2X<Type> other) const { return (x == other.x && y == other.y); };

	double distance(cocos2d::Vec2 other) const { return other.distance(*this); }

	double distanceSquared(Vec2X<Type> other) const {
		return (other.x - x) * (other.x - x) + (other.y - y) * (other.y - y);
	}

	// Swaps the x and y coordinates
	Vec2X<Type> swap() const { return Vec2X<Type>(y, x); }

	// Returns both components positive
	Vec2X<Type> positive() const { return Vec2X<Type>(ABS(x), ABS(y)); }

	// Returns x+(y*width)
	Type expand(Type width) const { return (x + (y*width)); }

	// The sum of the components
	Type sum() const { return x + y; }
	
	// returns true if any of the components are equal to the value
	bool componentEquals(int other) const { return (x == other || y == other); }
	
	// Returns the angle away from the x axis in radians
	double absoluteAngle() const {
		double angle = std::atan(y / x);
		if (x < 0) {
			angle = std::fmod((angle + PI), (2 * PI));
		}
		else {
			angle = std::fmod(angle + (2 * PI), (2 * PI));
		}
		return angle;
	}

	Type length() const{
		return std::sqrt(x*x + y*y);
	}

	unsigned int hash() const{
		return ((x * 51071) ^ (y * 61667)) & 65535;
	}

	Vec2X<Type> normalize() {
		Vec2X<Type> newVec = this->operator/(length());
		x = newVec.x;
		y = newVec.y;
		return *this;
	}

	Type dot(Vec2X<Type> other) const {
		return x*other.x + y*other.y;
	}

	Vec2X<Type> operator+(Vec2X<Type> other) const { return Vec2X<Type>(x + other.x, y + other.y); };
	Vec2X<Type> operator-(Vec2X<Type> other) const { return Vec2X<Type>(x - other.x, y - other.y); }

	bool operator==(Vec2X<Type> other) const { return equals(other); }
	bool operator!=(Vec2X<Type> other) const { return !equals(other); }
	Vec2X<Type> operator*(Type other) const { return Vec2X<Type>(x * other, y * other); }
	Vec2X<Type> operator/(Type other) const { return Vec2X<Type>(x / other, y / other); }

	bool operator<(Vec2X<Type> other) const { return hash() < other.hash(); }

	operator cocos2d::Vec2()const { return cocos2d::Vec2(x, y); }

};

typedef Vec2X<int> Vec2i;
typedef Vec2X<unsigned int> Vec2ui;
typedef Vec2X<float> Vec2f;
typedef Vec2X<unsigned long int> Vec2ul;
typedef Vec2X<unsigned long long int> Vec2ull;
typedef Vec2X<double> Vec2d;

namespace std {
	template <>
	struct hash<Vec2i> {
		std::size_t operator()(const Vec2i& vector) const {
			using std::size_t;
			using std::hash;

			return hash<int>()(vector.x) ^ hash<int>()(vector.y);
		}
	};
	template<>
	struct hash<Vec2ull> {
		std::size_t operator()(const Vec2ull& vector) const {
			using std::size_t;
			using std::hash;

			return hash<long long int>()(vector.x) ^ hash<long long int>()(vector.y);
		}
	};
}
