#include "cocos2d.h"
#include "Mortal.h"
class Puppet : public Mortal{
public:
	// creates a puppet based off the index of it's class
	static Puppet* create(int classIndex, std::string playerName, int playerIndex);	

	// Moves the puppet to a location
	virtual double moveTo(Vec2i position, unsigned char distance, float speed);

	// Show interactions
	void showInteractions();
protected:
	virtual void update(float delta);
private:

	bool m_active = false;

	virtual bool init(int classIndex, std::string playerName, int playerIndex);
};