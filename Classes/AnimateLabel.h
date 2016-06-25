#pragma once
#include "XBMPLabel.h"

class AnimateLabel : public XBMPLabel{
public:
	static AnimateLabel* create(std::string text, std::string font, int textSize, TextAlignment, double speed, double time);
	virtual bool init(std::string text, std::string font, int textSize, TextAlignment, double speed, double time);
	virtual void update(float delta);
	void dispose();
	bool isActive();
protected:
	double m_timeToDisplay;
	std::string m_animatedText;
	double m_animatedSpeed;
	double timeCount;
};