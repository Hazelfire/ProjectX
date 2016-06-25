#include "cocos2d.h"

class ErrorScreen : public cocos2d::Scene {
public:
	static ErrorScreen* create(std::string error);
private:
	virtual bool init(std::string error);
};