#include <vector>
#ifndef PERLINNOISE_H
#define PERLINNOISE_H

class PerlinNoise {
	std::vector<int> p;
public:
	// Initialize with the reference values for the permutation vector
	PerlinNoise();
	// Generate a new permutation vector based on the value of seed
	PerlinNoise(unsigned int seed);
	// Get a noise value, for 2D images z can have any value
	double noise(double x,double y);
private:
	double lerp(double t, double a, double b);
	double dotGridGradient(int ix, int iy, float x, float y);
	int m_scale;
	int fastfloor(double x);
	double dot(int g[], double x, double y);

};

#endif
