#include <cocos2d.h>

using namespace cocos2d;

class PackageManagerScene : public Scene {
public:
	static PackageManagerScene* create();
private:
	bool init();

	void createPackageSelector();
};