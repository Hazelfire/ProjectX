#include "cocos2d.h"

class Dialogue {
public:
	static void promptPlayer(std::string message, std::string placeholder, std::function<void(std::string)> callback, cocos2d::Node* runningScene);
	static void promptPlayer(std::string message, std::function<void(bool)> callBack, cocos2d::Node* runningScene);
};