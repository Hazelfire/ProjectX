#include <cocos2d.h>

class Health : public cocos2d::Node {
public:
	static Health* create();
	static Health* getInstance();
	double getHealth();
	void heal(double health);
	void damage(double health);
	void setHealth(double health);
private:
	bool init();
	static Health* m_healthInstance;
	double m_health = 100.0;
};