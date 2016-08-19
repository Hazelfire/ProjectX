#include "Mailbox.h"
#include "Parser/StringOperations.h"
#include "Parser/rapidxml.hpp"
#include "Parser/rapidxml_print.hpp"
#include <sstream>


// All the different things the Mailbox can control
#include "Inventory.h"

void Mailbox::giveItem(std::string itemName, int quantity) {
	m_scheduledItems.push_back({
		"Give Item",
		{ itemName ,StringOps::to_string(quantity)}});
}

void Mailbox::update(double delta) {

	// Lets start by sending all of our messages
	std::list<ScheduledItem> readyItems;

	auto scheduledItem = m_scheduledItems.begin();
	while (scheduledItem != m_scheduledItems.end()){
		readyItems.push_back(*scheduledItem);
		m_scheduledItems.erase(scheduledItem++);
	}

	// Sending them off!
	sendItems(readyItems);

	// Coolzies, now we should check if we have mail
	resolveInbox();
}

ScheduledItem getMessage(rapidxml::xml_node<>* messageTag) {
	ScheduledItem re;
	re.itemCommand = messageTag->first_attribute("name")->value();
	for (rapidxml::xml_node<>* paramaterNode = messageTag->first_node("par"); paramaterNode; paramaterNode->next_sibling()) {
		re.paramaters.push_back(paramaterNode->value());
	}

	return re;
}

void Mailbox::resolveInbox() {

	// For every message
	for (auto message : m_inbox) {
		rapidxml::xml_document<> doc;

		// Convert to C String
		char* cStr = new char[message.size() + 1];
		strcpy(cStr, message.c_str());
		doc.parse<0>(cStr);

		// For every event in message
		for (rapidxml::xml_node<>* eventNode = doc.first_node("message")->first_node("event"); eventNode; eventNode->next_sibling()) {
			ScheduledItem item = getMessage(eventNode);

			// Run the event
			excecuteItem(item);
		}
		delete cStr;

	}
}

void Mailbox::excecuteItem(ScheduledItem item) {
	// Here is where we run the scheduled items, this function will get very large
	if (item.itemCommand == "Give Item") {
		Inventory::giveItem(item.paramaters[0], std::stoi(item.paramaters[1]));
	}
}

void Mailbox::sendItems(std::list<ScheduledItem> scheduledItems) {
	// We have the items to send, we need to turn it into XML first, serialize it.
	using namespace rapidxml;

	xml_document<> doc;
	xml_node<>* xmlDeclaration = doc.allocate_node(node_declaration);
	xmlDeclaration->append_attribute(doc.allocate_attribute("version", "1.0"));
	xmlDeclaration->append_attribute(doc.allocate_attribute("type", "example"));
	doc.append_node(xmlDeclaration);

	xml_node<>* messageTag = doc.allocate_node(node_element, "message");
	for (auto item : scheduledItems) {
		xml_node<>* eventTag = doc.allocate_node(node_element, "event");
		eventTag->append_attribute(doc.allocate_attribute("name", item.itemCommand.c_str()));

		// Now we append the parameters
		for (auto paramater : item.paramaters) {
			xml_node<>* paramaterTag = doc.allocate_node(node_element, "par");
			paramaterTag->append_node(doc.allocate_node(node_data, "", paramater.c_str()));
			eventTag->append_node(paramaterTag);
		}
		messageTag->append_node(eventTag);
	}

	doc.append_node(messageTag);

	std::stringstream docStream;
	docStream << doc;
	
	// now we can send them to ourself, into our inbox
	inboxMessage(docStream.str());
}

void Mailbox::inboxMessage(std::string message) {
	m_inbox.push_back(message);
}