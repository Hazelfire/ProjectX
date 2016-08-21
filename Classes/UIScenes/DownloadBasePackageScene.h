#include <cocos2d.h>

class DownloadBasePackageScene : public cocos2d::Scene {
public:
	static DownloadBasePackageScene* create();
	virtual void update(float delta);
	static void startDownload();
private:
	bool init();
};