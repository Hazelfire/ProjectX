#include <string>

class Package {
public:
	Package(std::string folder);
	std::string getXmlScriptIndex();
	std::string getLuaScriptIndex();

	std::string getFolder();
	std::string getFolderFull();
	std::string getVersion();
	std::string getName();
	std::string getAuthor();
private:
	std::string m_folder;
	std::string m_name;
	std::string m_author;
	std::string m_version;
};