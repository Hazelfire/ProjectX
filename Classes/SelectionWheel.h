#include "cocos2d.h"

class SelectionWheel : public cocos2d::Node {
public:
	static SelectionWheel* create(std::list<std::string> options);
	bool init(std::list<std::string> options);
	void setSectionCallback(int index, std::function<void()> function);
	virtual void removeFromParentAndCleanup(bool state);
private:
	bool touchEvent(cocos2d::Touch* touch, cocos2d::Event* e);
	void callCallback(int index);
	int m_memberCount;
	std::vector<std::function<void()>> m_callbackFunctions;
	cocos2d::EventListenerTouchOneByOne* m_listener;
};