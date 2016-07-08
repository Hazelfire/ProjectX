#include <cocos2d.h>
#include "Lua\LuaGame.h"
#include "XBMPLabel.h"

class LuaTerminal : public cocos2d::Node {
public:
	static LuaTerminal* create();
	static LuaTerminal* getInstance();
private:
	bool init();
	
	void keyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);
	void keyReleased(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);

	void toggleActive();
	void appendKey(char key);
	void runCommand();

	LuaTerminal* m_terminalInstance = nullptr;

	bool m_active = false; 
	std::string m_currentCommand;
	LuaInterpreter m_interpreter;
	bool m_caps = false;
	XBMPLabel* m_commandField;
};