#include <string>
#include <list>
#include <vector>

struct ScheduledItem {
	std::string itemCommand;
	std::vector<std::string> paramaters;
};

class Mailbox {
public:
	void giveItem(std::string itemName, int quantity = 1);
	void update(double delta);
	void inboxMessage(std::string message);

private:

	void resolveInbox();
	void sendItems(std::list<ScheduledItem>);
	void excecuteItem(ScheduledItem);

	std::list<ScheduledItem> m_scheduledItems;

	std::list<std::string> m_inbox;
};