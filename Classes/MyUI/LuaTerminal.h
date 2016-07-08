#include <cocos2d.h>
#include "Lua\LuaGame.h"
#include "XBMPLabel.h"

class LuaTerminal : public cocos2d::Node {
public:
	static LuaTerminal* create();
	static LuaTerminal* getInstance();
	void print(std::string message);
private:
	bool init();
	
	void keyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);
	void keyReleased(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);

	void toggleActive();
	void appendKey(char key);
	void runCommand();

	static LuaTerminal* m_terminalInstance;

	bool m_active = false; 
	std::string m_currentCommand;
	LuaInterpreter m_interpreter;
	bool m_caps = false;

	std::list<std::string> m_logList;
	XBMPLabel* m_commandField;
};