#include <string>
#include "MyMath.h"
#include <unordered_map>
#include "YAML/ParticleParser.h"


class Particles {
public:
	static void spawnParticles(std::string particleSystemName, Vec2f location);
};