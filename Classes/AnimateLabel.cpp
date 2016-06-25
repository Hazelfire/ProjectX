#include "AnimateLabel.h"

AnimateLabel* AnimateLabel::create(std::string text, std::string font, int textSize, TextAlignment alignment, double speed, double time) {
	AnimateLabel *pRet = new(std::nothrow)AnimateLabel();
	if (pRet && pRet->init(text, font, textSize, alignment,speed, time))
	{
		pRet->autorelease();
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = nullptr;
		return nullptr;
	}
}
bool AnimateLabel::init(std::string text, std::string font, int textSize, TextAlignment alignment, double speed, double time) {
	m_animatedText = text;
	m_animatedSpeed = speed;
	m_timeToDisplay = time;
	if (!XBMPLabel::init("", font, textSize, alignment, 255, 0, Color3B::BLACK)) {
		return false;
	}
	scheduleUpdate();
	return true;
}
void AnimateLabel::update(float delta) {
	timeCount += delta;
	std::string textShown = m_animatedText.substr(0, std::min((int)m_animatedText.length(), (int)(timeCount / m_animatedSpeed)));
	m_string = textShown;
	reconstruct();
	if (m_timeToDisplay < timeCount) {
		dispose();
	}
}

void AnimateLabel::dispose() {
	unscheduleUpdate();
	removeFromParentAndCleanup(true);
	m_animatedSpeed = 0;
}

bool AnimateLabel::isActive() {
	if (m_animatedSpeed == 0) {
		return false;
	}
	else {
		return true;
	}
}