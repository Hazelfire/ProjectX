#include "cocos2d.h"

class Dialogue {
public:
	static void promtPlayer(std::string message, std::string placeholder, std::function<void(std::string)> callback, cocos2d::Node* runningScene);
};