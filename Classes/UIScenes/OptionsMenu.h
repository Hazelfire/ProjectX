#include "cocos2d.h"
#include "ui/CocosGUI.h"

class OptionsMenu : public cocos2d::Scene {
public:
	static OptionsMenu* create();
private:
	bool init();

	void inputTextCallback(cocos2d::Ref* reference, cocos2d::ui::TextField::EventType event);
};