#include "GitRepository.h"

#define SAFE_CALL(__ARGS__) if(__ARGS__){ m_lastError = giterr_last()->message; return false;}

GitRepository::GitRepository() {
	git_libgit2_init();
}

GitRepository::GitRepository(std::string repository){
	git_libgit2_init();

	git_repository_open(&m_repo, repository.c_str());
}

GitRepository::~GitRepository() {
	git_libgit2_shutdown();
}

bool GitRepository::addRemote(std::string remote, std::string url) {
	git_libgit2_init();

	git_remote* remoteP;
	SAFE_CALL(git_remote_create(&remoteP, m_repo, remote.c_str(), url.c_str()));

	git_remote_free(remoteP);

	git_libgit2_shutdown();

	return true;
}

bool GitRepository::hasRemote(std::string remote) {
	git_libgit2_init();

	git_remote* remoteP;
	if (git_remote_lookup(&remoteP, m_repo, remote.c_str())) {
		return false;
	}
	
	git_remote_free(remoteP);

	git_libgit2_shutdown();

	return true;
}

bool GitRepository::fetch(std::string remote) {

	git_remote* gitRemote;
	SAFE_CALL(git_remote_lookup(&gitRemote, m_repo, remote.c_str()));

	SAFE_CALL(git_remote_fetch(gitRemote, NULL, NULL, NULL));

	git_remote_free(gitRemote);
	return true;
}

bool GitRepository::merge(std::string branch){
	/*git_reference* remoteBranch;
	SAFE_CALL(git_branch_lookup(&remoteBranch, m_repo, branch.c_str(), GIT_BRANCH_REMOTE));
	
	const git_oid* remoteCommit = git_reference_target(remoteBranch);

	git_annotated_commit* originHead;

	git_annotated_commit_lookup(&originHead, m_repo, remoteCommit);

	git_merge(m_repo, &originHead, 1, NULL, NULL);*/
	return true;
}

int fetch_progress(const git_transfer_progress* stats, void* payload) {
	GitRepository* client = (GitRepository*)payload;

	GitRepository::DownloadProgress progress;
	progress.receivedObjects = stats->received_objects;
	progress.recievedBytes = stats->received_bytes;
	progress.totalObjects = stats->total_objects;
	client->setDownloadProgress(progress);

	return 0;
}

void checkout_progress(const char* path, size_t cur, size_t tot, void* payload) {
	GitRepository* client = (GitRepository*)payload;

	GitRepository::CheckoutProgress progress;
	progress.completedSteps = cur;
	progress.totalSteps = tot;

	client->setCheckoutProgress(progress);
}


bool GitRepository::clone(std::string url, std::string folder) {
	git_clone_options options = GIT_CLONE_OPTIONS_INIT;

	options.checkout_opts.checkout_strategy = GIT_CHECKOUT_SAFE;
	options.checkout_opts.progress_cb = checkout_progress;
	options.checkout_opts.progress_payload = this;
	options.fetch_opts.callbacks.transfer_progress = fetch_progress;
	options.fetch_opts.callbacks.payload = this;

	SAFE_CALL(git_clone(&m_repo, url.c_str(), folder.c_str(), &options));

	m_active = true;

	return true;
}

std::string GitRepository::getError() {
	return m_lastError;
}


void GitRepository::setDownloadProgress(DownloadProgress progress) {
	m_downloadProgress = progress;
}

GitRepository::DownloadProgress GitRepository::getDownloadProgress() {
	return m_downloadProgress;
}

void GitRepository::setCheckoutProgress(CheckoutProgress progress) {
	m_checkoutProgress = progress;
}

GitRepository::CheckoutProgress GitRepository::getCheckoutProgress() {
	return m_checkoutProgress;
}