#include "Particles.h"
#include "cocos2d.h"
#include "Arena.h"
#include "ResourceMacros.h"
#include "ScriptLoader.h"
#include "Packages/PackageManager.h"

void Particles::spawnParticles(std::string particleSystemName, Vec2f location) {
	
	std::string filePath = PackageManager::getInstance()->getFile("res/particles/" + particleSystemName + ".plist");

	auto cocosParticleSystem = cocos2d::ParticleSystemQuad::create(filePath);
	cocosParticleSystem->setScale(0.5);
	Arena::getMapInstance()->placeNode(location, cocosParticleSystem);
	Arena::getMapInstance()->setZOrderOfNode(location, cocosParticleSystem, 1);
}
