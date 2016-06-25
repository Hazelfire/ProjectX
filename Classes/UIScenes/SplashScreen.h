#include "cocos2d.h"

class SplashScreen : public cocos2d::Scene {
public:
	static SplashScreen* create();
private:
	virtual bool init();
};