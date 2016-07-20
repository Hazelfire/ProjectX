#include "PerlinNoise.h"
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>
#include <math.h>
#include "MyMath.h"
int grad3[12][3] = { { 1,1,0 },{ -1,1,0 },{ 1,-1,0 },{ -1,-1,0 },
{ 1,0,1 },{ -1,0,1 },{ 1,0,-1 },{ -1,0,-1 },
{ 0,1,1 },{ 0,-1,1 },{ 0,1,-1 },{ 0,-1,-1 } };
// THIS IS A DIRECT TRANSLATION TO C++11 FROM THE REFERENCE
// JAVA IMPLEMENTATION OF THE IMPROVED PERLIN FUNCTION (see http://mrl.nyu.edu/~perlin/noise/)
// THE ORIGINAL JAVA IMPLEMENTATION IS COPYRIGHT 2002 KEN PERLIN

// I ADDED AN EXTRA METHOD THAT GENERATES A NEW p.atUTATION VECTOR (THIS IS NOT PRESENT IN THE ORIGINAL IMPLEMENTATION)

// Initialize with the reference values for the p.atutation vector
PerlinNoise::PerlinNoise() {
	// Initialize the p.atutation vector with the reference values
	p = {
		151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
		8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
		250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
		189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 
		43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
		97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
		107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
	// Duplicate the p.atutation vector
	p.insert(p.end(), p.begin(), p.end());
}

int PerlinNoise::fastfloor(double x) {
	return x > 0 ? (int)x : (int)x - 1;
}

double PerlinNoise::dot(int g[], double x, double y) {
	return g[0] * x + g[1] * y;
}

double PerlinNoise::lerp(double a, double b, double t) {
	return (1-t)*a + t*b;
}

double fade(double t) {
	return t*t*t*(t *(t* 6 - 15) + 10);
}

void fill(std::vector<int>& fillingVector) {
	fillingVector.clear();
	for (int i = 0; i < 256; i++) {
		fillingVector.push_back(i);
	}
};

// a very simple shuffle, basically for every element, it randomly choses another element to swap with
void shuffle(std::vector<int>& vector, unsigned int seed) {

	for (int i = 0; i < vector.size(); i++) {

		int index = ((i * 53959) ^ (seed * 47297)) % vector.size();
		int temp = vector[index];
		vector[index] = vector[i];
		vector[i] = temp;
	}
}

PerlinNoise::PerlinNoise(unsigned int seed) {
	// Fill p with values from 0 to 255
	fill(p);

	// Suffle using seed
	shuffle(p, seed);

	// Duplicate the p.atutation vector
	p.insert(p.end(), p.begin(), p.end());
}

double PerlinNoise::noise(double xin, double yin) {

	// This is because 0,0 will always be 50, or average. This compensates.
	xin += 2;
	yin += 2;
	double n0, n1, n2; // Noise contributions from the three corners
					   // Skew the input space to determine which simplex cell we're in
	double F2 = 0.5*(sqrt(3.0) - 1.0);
	double s = (xin + yin)*F2; // Hairy factor for 2D
	int i = fastfloor(xin + s);
	int j = fastfloor(yin + s);
	double G2 = (3.0 - sqrt(3.0)) / 6.0;
	double t = (i + j)*G2;
	double X0 = i - t; // Unskew the cell origin back to (x,y) space
	double Y0 = j - t;
	double x0 = xin - X0; // The x,y distances from the cell origin
	double y0 = yin - Y0;
	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0>y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else { i1 = 0; j1 = 1; } // upper triangle, YX order: (0,0)->(0,1)->(1,1)
							 // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
							 // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
							 // c = (3-sqrt(3))/6
	double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	double y2 = y0 - 1.0 + 2.0 * G2;
	// Work out the hashed gradient indices of the three simplex corners
	int ii = i & 255;
	int jj = j & 255;
	int gi0 = p.at(ii + p.at(jj)) % 12;
	int gi1 = p.at(ii + i1 + p.at(jj + j1)) % 12;
	int gi2 = p.at(ii + 1 + p.at(jj + 1)) % 12;
	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0*x0 - y0*y0;
	if (t0<0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * dot(grad3[gi0], x0, y0); // (x,y) of grad3 used for 2D gradient
	}
	double t1 = 0.5 - x1*x1 - y1*y1;
	if (t1<0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
	}
	double t2 = 0.5 - x2*x2 - y2*y2;
	if (t2<0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
	}
	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return ((70.0 * (n0 + n1 + n2))+1)*50;
}

double PerlinNoise::dotGridGradient(int ix, int iy, float x, float y) {
	// Precomputed gradient vectors at each grid node

	Vec2f g(p.at(ix+1), p.at(iy+1+(p.size()/3)));
	g.normalize();
	// Compute the distance vector
	Vec2f d(x - (float)ix, y - (float)iy);

	// Compute the dot-product
	return g.dot(d);
}
