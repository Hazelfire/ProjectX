#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"

#define NOT_MOBILE (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID && CC_TARGET_PLATFORM != CC_PLATFORM_IOS)


class HelloWorld : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

	// Keyboard listener the listens to the escape key (for close)
	static void globalKeyListener(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);

	// Asks for the players name if they have none
	void queryPlayerName();
};

#endif // __HELLOWORLD_SCENE_H__
