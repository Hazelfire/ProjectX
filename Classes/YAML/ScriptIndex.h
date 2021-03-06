#include <string>
#include <list>


class ScriptIndex {
public:
	ScriptIndex() {};
	ScriptIndex(std::string scriptIndex);

	std::list<std::string> getFiles(std::string directory);

	std::string getDirectory();
	std::string getPath();

private:
	// Creates the scipt indexes based on what is in the files
	static std::string createScriptIndex(std::string fileName);

	std::list<std::string> m_files;

	std::string m_indexPath;
};