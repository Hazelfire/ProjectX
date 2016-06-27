#include "XBMPLabel.h"
#include "ResourceMacros.h"
#include "Utils.h"
#include "Debug.h"

XBMPLabel::FontMap XBMPLabel::m_cachedFonts;

XBMPLabel* XBMPLabel::create(std::string text, std::string font, int textSize, TextAlignment alignment, unsigned char opacity, int wrap, Color3B color){
	XBMPLabel *pRet = new(std::nothrow)XBMPLabel();
	if (pRet && pRet->init(text, font, textSize, alignment, opacity, wrap, color))
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

bool XBMPLabel::init(std::string text, std::string font, int textSize, TextAlignment alignment, unsigned char opacity, int wrap, Color3B color){
	if (!Node::init()) {
		return false;
	}


	m_string = text;
	m_fontFile = font;
	m_textSize = textSize;
	m_textAlignment = alignment;
	m_opacity = opacity;
	m_wrap = wrap;
	m_colour = color;

	if (m_cachedFonts.find(font) == m_cachedFonts.end()) {
		m_cachedFonts[font] = FontParser::parse(FONTS_FOLDER + m_fontFile + ".xfnt");
	}
	m_fontInfo = m_cachedFonts[font];
	int horizontalTextOffset=0;
	int textWidth = 0;
	int characterHieght = 0;
	int lineNumber = 0;

	Node* textSurface = Node::create();

	for (std::string::iterator currentCharacter = text.begin(); currentCharacter != text.end(); currentCharacter++) {
		if (*currentCharacter == ' ') {
			horizontalTextOffset += m_fontInfo.spaceDistance;
			continue;
		}
		// If the character is newline or word wrap is required
		else if (*currentCharacter == '\n' || (wrap != 0 && characterHieght != 0 &&((horizontalTextOffset * (textSize / characterHieght)) / CC_CONTENT_SCALE_FACTOR()) > wrap)) {
			horizontalTextOffset = 0;
			lineNumber++;
		}
		else if (*currentCharacter == '\t') {
			continue;
		}
		Sprite* character = Sprite::create(m_fontInfo.pictureFile,CC_RECT_PIXELS_TO_POINTS( m_fontInfo.characterLocations[*currentCharacter]));
		if (character) {
			character->setAnchorPoint(Vec2(0, 0));
			character->setPosition(Vec2(horizontalTextOffset, -lineNumber * characterHieght));
			if (!m_fontInfo.antialias) character->getTexture()->setAliasTexParameters();

			horizontalTextOffset += m_fontInfo.characterLocations[*currentCharacter].size.width;
			if (horizontalTextOffset > textWidth) {
				textWidth = horizontalTextOffset;
			}
			textSurface->addChild(character);
			if (characterHieght < m_fontInfo.characterLocations[*currentCharacter].size.height) {
				characterHieght = m_fontInfo.characterLocations[*currentCharacter].size.height;
			}
			character->setOpacity(opacity);
			character->setColor(Color3B(0, 0, 0));
		}
		else {
			Debugger::logWarning(std::string("Failed to create sprite for letter ") + *currentCharacter + " in font " + font, DEBUG_FONTS);
		}
	}
	m_textSurface = textSurface;

	textSurface->setContentSize(Size(textWidth, characterHieght * (lineNumber + 1)));
	textSurface->setAnchorPoint(Vec2(0, 0));
	addChild(textSurface);
	m_textSize = characterHieght;
	EventListenerTouchOneByOne* listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(XBMPLabel::callBack, this);
	listener->setSwallowTouches(true);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	m_lines = lineNumber + 1;

	// Font size
	setSize(textSize);
	setAnchorPoint(Vec2(0.5, 0.5));

	// alignments
	switch (alignment) {
	case LEFT:
		setAnchorPoint(Vec2(0, 0.5));
		break;
	case CENTER:
		setAnchorPoint(Vec2(0.5, 0.5));
		break;
	case RIGHT:
		setAnchorPoint(Vec2(1, 0.5));
		break;
	default: // If there IS NO SUCH ALIGNMENT
		return false;
	}

	return true;
}

void XBMPLabel::setCallback(std::function<void()> callback) {
	m_hasCallback = true;
	m_callback = callback;
}

bool XBMPLabel::callBack(cocos2d::Touch* touch, cocos2d::Event* e){
	if (!m_hasCallback) return false;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 touchLocation = touch->getLocation();

	Rect worldRect = Utils::getWorldRect(m_textSurface);
	if (worldRect.containsPoint(touchLocation)) {
		m_callback();
		return true;
	}
	return false;
}

void XBMPLabel::setSize(int scale) {
	m_textSurface->setScale(scale/m_textSurface->getContentSize().height * m_lines);
	setContentSize(m_textSurface->getContentSize() * m_textSurface->getScale());
	m_textSize = scale;
}

void XBMPLabel::reconstruct() {
	removeAllChildrenWithCleanup(true);
	init(m_string, m_fontFile,m_textSize,m_textAlignment,m_opacity,m_wrap, m_colour);
}
void XBMPLabel::setString(std::string text) {
	m_string = text;
	reconstruct();
}
