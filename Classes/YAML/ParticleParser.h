#include <base/ccTypes.h>
#include <string>
#include <list>
class ParticleParser {
public:
	enum ParticleSystemMode {
		GRAVITY
	};

	struct ParticleSystem {
		std::string name;
		float duration = 3;
		int totalParticles = 100;
		ParticleSystemMode mode = GRAVITY;
		cocos2d::Vec2 gravity;
		float radialAcceleration = 0;
		float radialAccelerationVar = 0;
		float speed = 0;
		float speedVar = 0;
		float angle = 0;
		float angleVar = 0;
		float life = 3;
		float lifeVar = 0;
		float startSize = 50;
		float startSizeVar = 0;
		float endSize = 50;
		float endSizeVar = 0;
		cocos2d::Color4F startColour;
		cocos2d::Color4F startColourVar;
		cocos2d::Color4F endColour;
		cocos2d::Color4F endColourVar;
		std::string texture;
	};

	typedef std::list<ParticleSystem> ParticleSystemList;

	static ParticleSystemList parse(std::string source);
	
};