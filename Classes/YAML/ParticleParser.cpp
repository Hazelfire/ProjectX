#include "ParticleParser.h"
#include <yaml-cpp\yaml.h>
#include "Debug.h"
#include "Parser/StringOperations.h"

ParticleParser::ParticleSystemList ParticleParser::parse(std::string source) {
	ParticleSystemList re;

	try {
		YAML::Node root = YAML::Load(source);

		for (auto systemNode : root) {
			
			ParticleSystem currentSystem;
			currentSystem.name = systemNode.first.as<std::string>();
			
			if (systemNode.second["duration"])
				currentSystem.duration = systemNode.second["duration"].as<float>();

			if (systemNode.second["mode"]) {
				if (systemNode.second["mode"].as<std::string>() == "gravity") {
					currentSystem.mode = GRAVITY;
				}
			}

			if (systemNode.second["totalParticles"])
				currentSystem.totalParticles = systemNode.second["totalParticles"].as<int>();

			if (systemNode.second["gravity"]) {
				float gravityX = systemNode.second["gravity"][0].as<float>();
				float gravityY = systemNode.second["gravity"][1].as<float>();
				currentSystem.gravity = cocos2d::Vec2(gravityX, gravityY);
			}

			if (systemNode.second["radialAcceleration"])
				currentSystem.radialAcceleration = systemNode.second["radialAcceleration"].as<float>();

			if (systemNode.second["radialAccelerationVar"])
				currentSystem.radialAccelerationVar = systemNode.second["radialAccelerationVar"].as<float>();

			if (systemNode.second["speed"])
				currentSystem.speed = systemNode.second["speed"].as<float>();

			if (systemNode.second["speedVar"])
				currentSystem.speedVar = systemNode.second["speedVar"].as<float>();

			if (systemNode.second["angle"])
				currentSystem.angle = systemNode.second["angle"].as<float>();

			if (systemNode.second["angleVar"])
				currentSystem.angleVar = systemNode.second["angleVar"].as<float>();

			if (systemNode.second["life"])
				currentSystem.life = systemNode.second["life"].as<float>();

			if (systemNode.second["lifeVar"])
				currentSystem.life = systemNode.second["lifeVar"].as<float>();

			if (systemNode.second["startSize"])
				currentSystem.startSize = systemNode.second["startSize"].as<float>();

			if (systemNode.second["startSizeVar"])
				currentSystem.startSizeVar = systemNode.second["startSizeVar"].as<float>();

			if (systemNode.second["endSize"]) {
				if (systemNode.second["endSize"].as<std::string>() == "sameAsStart") {
					currentSystem.endSize = -1;
				}
				else {
					currentSystem.endSize = systemNode.second["endSize"].as<float>();
				}
			}

			if (systemNode.second["endSizeVar"])
				currentSystem.endSizeVar = systemNode.second["endSizeVar"].as<float>();

			if (systemNode.second["startColour"]) {
				float r = systemNode.second["startColour"][0].as<float>();
				float g = systemNode.second["startColour"][1].as<float>();
				float b = systemNode.second["startColour"][2].as<float>();
				float a = systemNode.second["startColour"][3].as<float>();
				currentSystem.startColour = cocos2d::Color4F(r, g, b, a);
			}

			if (systemNode.second["startColourVar"]) {
				float r = systemNode.second["startColourVar"][0].as<float>();
				float g = systemNode.second["startColourVar"][1].as<float>();
				float b = systemNode.second["startColourVar"][2].as<float>();
				float a = systemNode.second["startColourVar"][3].as<float>();
				currentSystem.startColourVar = cocos2d::Color4F(r, g, b, a);
			}

			if (systemNode.second["endColour"]) {
				float r = systemNode.second["endColour"][0].as<float>();
				float g = systemNode.second["endColour"][1].as<float>();
				float b = systemNode.second["endColour"][2].as<float>();
				float a = systemNode.second["endColour"][3].as<float>();
				currentSystem.endColour = cocos2d::Color4F(r, g, b, a);
			}

			if (systemNode.second["endColourVar"]) {
				float r = systemNode.second["endColourVar"][0].as<float>();
				float g = systemNode.second["endColourVar"][1].as<float>();
				float b = systemNode.second["endColourVar"][2].as<float>();
				float a = systemNode.second["endColourVar"][3].as<float>();
				currentSystem.endColourVar = cocos2d::Color4F(r, g, b, a);
			}

			// not being safe about texture, particles need a texture to work
			currentSystem.texture = systemNode.second["texture"].as<std::string>();


			// finally, add it to the list of particles
			re.push_back(currentSystem);
		}
	}
	catch (YAML::Exception e) {
		Debugger::logError("Failed parsing YAML Particles file with error " + e.msg + " at [" + StringOps::to_string(e.mark.line) + ", " + StringOps::to_string(e.mark.column) + "]", DEBUG_YAML);
	}

	return re;
}