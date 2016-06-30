#include <string>
#include "MyMath.h"
#include <unordered_map>

class Particles {
public:
	static void spawnParticles(std::string particleSystemName, Vec2f location, float duration = 1.0f);
};