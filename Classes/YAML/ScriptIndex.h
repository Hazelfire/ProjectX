#include <string>
#include <list>


class ScriptParser {
public:
	// Creates the scipt indexes based on what is in the files
	static std::string createScriptIndex(std::string fileName);

	struct XmlScriptIndex {
		std::list<std::string> creatures;
		std::list<std::string> interactions;
		std::list<std::string> items;
		std::list<std::string> map;
		std::list<std::string> music;
		std::list<std::string> players;
		std::list<std::string> sprites;
		std::list<std::string> schematics;
	};

	static XmlScriptIndex parseXml(std::string fileName);
	
	struct LuaScriptIndex {
		std::list<std::string> creatures;
		std::list<std::string> interactions;
		std::list<std::string> startup;
		std::list<std::string> items;
	};

	static bool m_directoriesChecked;

	static LuaScriptIndex parseLua(std::string fileName);
};