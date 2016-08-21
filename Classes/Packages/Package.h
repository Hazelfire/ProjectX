#include <string>
#include "GitRepository.h"

class Package {
public:

	// Opens if exists, new if does not
	Package(std::string folder);
	~Package() { delete m_gitRepo; };

	void setUrl(std::string url);

	bool sync();
	void lockDownload();
	bool isDownloading();

	std::pair<GitRepository::DownloadProgress, GitRepository::CheckoutProgress> getSyncProgress();

	std::string getXmlScriptIndex();
	std::string getLuaScriptIndex();

	std::string getFolder();
	std::string getFolderFull();
	std::string getVersion();
	std::string getName();
	std::string getAuthor();
private:
	bool m_active = false;
	GitRepository* m_gitRepo = nullptr;
	std::string m_folder;
	std::string m_name;
	std::string m_author;
	std::string m_version;
	std::string m_remote;

	bool m_isDownloading;
};