#include <cocos2d.h>
#include "Lua/LuaPersonal.h"
#include <vector> 

class LuaTerminal : public cocos2d::Node {
public:
	static LuaTerminal* create();
	static LuaTerminal* getInstance();
	void print(std::string message);

	void clear();
private:
	bool init();
	
	void keyPressed(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);
	void keyReleased(cocos2d::EventKeyboard::KeyCode, cocos2d::Event*);

	void updateCommandString();

	void refreshTerminal();

	void toggleActive();
	void appendKey(char key);
	void runCommand();

	static LuaTerminal* m_terminalInstance;

	bool m_active = false; 
	std::string m_currentCommand;
	LuaPersonal m_interpreter;
	bool m_caps = false;

	std::list<std::string> m_logList;
	int m_commandIndex = -1;
	std::vector<std::string> m_commandList;
	cocos2d::LabelTTF* m_commandField;
	bool m_continuationMode = false;

};
