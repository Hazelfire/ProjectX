#include "LuaTerminal.h"

LuaTerminal* LuaTerminal::m_terminalInstance = nullptr;
using namespace cocos2d;

LuaTerminal* LuaTerminal::create() {
	LuaTerminal* re = new (std::nothrow) LuaTerminal();
	if (re && re->init()) {
		re->autorelease();
		m_terminalInstance = re;
		return re;
	}
	else {
		delete re;
		re = nullptr;
		return nullptr;
	}
}

bool LuaTerminal::init() {
	Size visibleSize = Director::getInstance()->getVisibleSize();

	// Create the key listener
	EventListenerKeyboard* keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(LuaTerminal::keyPressed, this);
	keyListener->onKeyReleased = CC_CALLBACK_2(LuaTerminal::keyReleased, this);

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyListener, this);

	m_commandField = XBMPLabel::create("<Enter command>", "Pixelfont", 20, XBMPLabel::LEFT);

	Vec2 commandPosition = Vec2(visibleSize.height / 10, visibleSize.height / 10);
	m_commandField->setPosition(commandPosition);
	addChild(m_commandField);

	setVisible(false);
	return true;

}

LuaTerminal* LuaTerminal::getInstance() {
	return m_terminalInstance;
}

void LuaTerminal::keyPressed(EventKeyboard::KeyCode code, Event*) {
	if (!m_active) {
		if (code == EventKeyboard::KeyCode::KEY_T) {
			toggleActive();
		}
	}
	else {
		if (code == EventKeyboard::KeyCode::KEY_ESCAPE) {
			toggleActive();
		}
		else if (code == EventKeyboard::KeyCode::KEY_ENTER) {
			runCommand();
		}
		else if (code == EventKeyboard::KeyCode::KEY_BACKSPACE) {
			m_currentCommand.pop_back();
			m_commandField->setString(m_currentCommand);
		}
		else if (code == EventKeyboard::KeyCode::KEY_CAPS_LOCK) {
			m_caps = !m_caps;
		}
		else if (code == EventKeyboard::KeyCode::KEY_SHIFT) {
			m_caps = !m_caps;
		}
		else {
			switch (code) {
			case EventKeyboard::KeyCode::KEY_0:
				appendKey(m_caps? ')': '0');
				break;
			case EventKeyboard::KeyCode::KEY_1:
				appendKey(m_caps ? '!' : '1');
				break;
			case EventKeyboard::KeyCode::KEY_2:
				appendKey(m_caps ? '@' : '2');
				break;
			case EventKeyboard::KeyCode::KEY_3:
				appendKey(m_caps ? '#' : '3');
				break;
			case EventKeyboard::KeyCode::KEY_4:
				appendKey(m_caps ? '$' : '4');
				break;
			case EventKeyboard::KeyCode::KEY_5:
				appendKey(m_caps ? '%' : '5');
				break;
			case EventKeyboard::KeyCode::KEY_6:
				appendKey(m_caps ? '^': '6');
				break;
			case EventKeyboard::KeyCode::KEY_7:
				appendKey(m_caps ? '&' : '7');
				break;
			case EventKeyboard::KeyCode::KEY_8:
				appendKey(m_caps ? '*' : '8');
				break;
			case EventKeyboard::KeyCode::KEY_9:
				appendKey(m_caps ? '(' : '9');
				break;
			case EventKeyboard::KeyCode::KEY_APOSTROPHE:
				appendKey(m_caps ? '"' : '\'');
				break;
			case EventKeyboard::KeyCode::KEY_BACK_SLASH:
				appendKey(m_caps ? '|' : '\\');
				break;
			case EventKeyboard::KeyCode::KEY_COMMA:
				appendKey(m_caps ? '<' : ',');
				break;
			case EventKeyboard::KeyCode::KEY_EQUAL:
				appendKey(m_caps ? '+' : '=');
				break;
			case EventKeyboard::KeyCode::KEY_GRAVE:
				appendKey(m_caps ? '~' : '`');
				break;
			case EventKeyboard::KeyCode::KEY_PERIOD:
				appendKey(m_caps ? '>' : '.');
				break;
			case EventKeyboard::KeyCode::KEY_LEFT_BRACKET:
				appendKey(m_caps ? '{' : '[');
				break;
			case EventKeyboard::KeyCode::KEY_RIGHT_BRACKET:
				appendKey(m_caps ? '}' : ']');
				break;
			case EventKeyboard::KeyCode::KEY_SEMICOLON:
				appendKey(m_caps ? ':' : ';');
				break;
			case EventKeyboard::KeyCode::KEY_SLASH:
				appendKey(m_caps ? '?' : '/');
				break;
			case EventKeyboard::KeyCode::KEY_MINUS:
				appendKey(m_caps ? '_' : '-');
				break;
			case EventKeyboard::KeyCode::KEY_A:
				appendKey(m_caps ? 'A' : 'a');
				break;
			case EventKeyboard::KeyCode::KEY_B:
				appendKey(m_caps ? 'B' : 'b');
				break;
			case EventKeyboard::KeyCode::KEY_C:
				appendKey(m_caps ? 'C' : 'c');
				break;
			case EventKeyboard::KeyCode::KEY_D:
				appendKey(m_caps ? 'D' : 'd');
				break;
			case EventKeyboard::KeyCode::KEY_E:
				appendKey(m_caps ? 'E' : 'e');
				break;
			case EventKeyboard::KeyCode::KEY_F:
				appendKey(m_caps ? 'F' : 'f');
				break;
			case EventKeyboard::KeyCode::KEY_G:
				appendKey(m_caps ? 'G' : 'g');
				break;
			case EventKeyboard::KeyCode::KEY_H:
				appendKey(m_caps ? 'H' : 'h');
				break;
			case EventKeyboard::KeyCode::KEY_I:
				appendKey(m_caps ? 'I' : 'i');
				break;
			case EventKeyboard::KeyCode::KEY_J:
				appendKey(m_caps ? 'J' : 'j');
				break;
			case EventKeyboard::KeyCode::KEY_K:
				appendKey(m_caps ? 'K' : 'k');
				break;
			case EventKeyboard::KeyCode::KEY_L:
				appendKey(m_caps ? 'L' : 'l');
				break;
			case EventKeyboard::KeyCode::KEY_M:
				appendKey(m_caps ? 'M' : 'm');
				break;
			case EventKeyboard::KeyCode::KEY_N:
				appendKey(m_caps ? 'N' : 'n');
				break;
			case EventKeyboard::KeyCode::KEY_O:
				appendKey(m_caps ? 'O' : 'o');
				break;
			case EventKeyboard::KeyCode::KEY_P:
				appendKey(m_caps ? 'P' : 'p');
				break;
			case EventKeyboard::KeyCode::KEY_Q:
				appendKey(m_caps ? 'Q' : 'q');
				break;
			case EventKeyboard::KeyCode::KEY_R:
				appendKey(m_caps ? 'R' : 'r');
				break;
			case EventKeyboard::KeyCode::KEY_S:
				appendKey(m_caps ? 'S' : 's');
				break;
			case EventKeyboard::KeyCode::KEY_T:
				appendKey(m_caps ? 'T' : 't');
				break;
			case EventKeyboard::KeyCode::KEY_U:
				appendKey(m_caps ? 'U' : 'u');
				break;
			case EventKeyboard::KeyCode::KEY_V:
				appendKey(m_caps ? 'V' : 'v');
				break;
			case EventKeyboard::KeyCode::KEY_W:
				appendKey(m_caps ? 'W' : 'w');
				break;
			case EventKeyboard::KeyCode::KEY_X:
				appendKey(m_caps ? 'X' : 'x');
				break;
			case EventKeyboard::KeyCode::KEY_Y:
				appendKey(m_caps ? 'Y' : 'y');
				break;
			case EventKeyboard::KeyCode::KEY_Z:
				appendKey(m_caps ? 'Z' : 'z');
				break;
			case EventKeyboard::KeyCode::KEY_SPACE:
				appendKey(' ');
				break;
			}
		}
	}
}


void LuaTerminal::keyReleased(cocos2d::EventKeyboard::KeyCode code, cocos2d::Event*) {
	if (code == EventKeyboard::KeyCode::KEY_SHIFT) {
		m_caps = !m_caps;
	}
}

void LuaTerminal::print(std::string message) {
	m_logList.push_front(message);

	Size visibleSize = Director::getInstance()->getVisibleSize();

	Vec2 startPosition = Vec2((visibleSize.height / 10), visibleSize.height / 10 + 20);

	Node* termDisplay = Node::create();
	termDisplay->setName("Display");
	termDisplay->setPosition(startPosition);

	int i = 0;
	for (auto currentLog : m_logList) {
		XBMPLabel* currentLogLabel = XBMPLabel::create(currentLog, "PixelFont", 20, XBMPLabel::LEFT);
		currentLogLabel->setPosition(Vec2(0, 20*i));
		termDisplay->addChild(currentLogLabel);
		i++;
	}

	if (getChildByName("Display")) {
		removeChildByName("Display");
	}
	addChild(termDisplay);
}

void LuaTerminal::toggleActive() {
	if (m_active) {
		setVisible(false);
	}
	else {
		setVisible(true);
	}
	m_active = !m_active;
}


void LuaTerminal::appendKey(char key) {
	m_currentCommand.push_back(key);
	m_commandField->setString(m_currentCommand);
}

void LuaTerminal::runCommand() {
	m_interpreter.run(std::list<std::string>(), m_currentCommand);
	m_currentCommand = "";
	m_commandField->setString("<Enter command>");
}