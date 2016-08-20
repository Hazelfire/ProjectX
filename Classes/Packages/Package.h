#include <string>

class Package {
public:
	Package(std::string folder);
	std::string getXmlScriptIndex();
	std::string getLuaScriptIndex();
	std::string getFolder();
private:
	std::string m_folder;
	std::string m_name;
	std::string m_author;
	int m_versionMajor;
	int m_versionMinor;
	int m_versionRevision;
};