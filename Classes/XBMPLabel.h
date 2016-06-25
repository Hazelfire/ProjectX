#pragma once
#include "cocos2d.h"
#include "Parser/FontParser.h"
USING_NS_CC;
class XBMPLabel : public Node {
public:
	typedef std::unordered_map<std::string, FontInformation> FontMap;
	enum TextAlignment {
		LEFT,
		CENTER,
		RIGHT
	};
	static XBMPLabel* create(std::string text, std::string font, int textSize = 11, TextAlignment aligmnent = CENTER, unsigned char opacity = 255, int wrap = 0, Color3B color = Color3B::BLACK);

	bool init(std::string text, std::string font, int textSize, TextAlignment aligmnent, unsigned char opacity, int wrap, Color3B color);

	void setCallback(std::function<void()>);
	bool callBack(cocos2d::Touch* touch, cocos2d::Event* e);
	void setSize(int scale);
	void setString(std::string text);
protected:
	int m_lines;
	std::string m_string;
	std::string m_fontFile;
	int m_textSize;
	TextAlignment m_textAlignment;
	unsigned char m_opacity;
	Color3B m_colour;
	int m_wrap;
	void reconstruct();
	bool m_hasCallback;
	Node* m_textSurface;
	static FontMap m_cachedFonts;
	FontInformation m_fontInfo;
	std::function<void()> m_callback;
};