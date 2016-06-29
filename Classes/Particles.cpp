#include "Particles.h"
#include "cocos2d.h"
#include "Arena.h"
#include "ResourceMacros.h"
#include "ScriptLoader.h"

void Particles::spawnParticles(std::string particleSystemName, Vec2f location) {
	
	auto cocosParticleSystem = cocos2d::ParticleSystemQuad::create(PARTICLES_FOLDER + particleSystemName + ".plist");
	Arena::getMapInstance()->placeNode(location, cocosParticleSystem);
	Arena::getMapInstance()->setZOrderOfNode(location, cocosParticleSystem, 1);
}