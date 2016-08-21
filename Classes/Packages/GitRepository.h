#pragma once
#include <string>
#include <git2.h>

class GitRepository {
public:
	struct DownloadProgress {
		int recievedBytes = 0;
		int totalObjects = 0;
		int receivedObjects = 0;
	};

	struct CheckoutProgress {
		int completedSteps = 0;
		int totalSteps = 0;
	};

	bool clone(std::string url, std::string folder);
	GitRepository();
	GitRepository(std::string repository);
	~GitRepository();
	bool addRemote(std::string remote, std::string url);

	bool hasRemote(std::string remote);

	bool fetch(std::string remote);

	bool merge(std::string branch);

	std::string getError();

	DownloadProgress getDownloadProgress();
	CheckoutProgress getCheckoutProgress();


	void setDownloadProgress(DownloadProgress);
	void setCheckoutProgress(CheckoutProgress);

private:
	bool m_active = false;
	git_repository* m_repo;
	std::string m_lastError;

	// progress
	DownloadProgress m_downloadProgress;
	CheckoutProgress m_checkoutProgress;
};