#include "cocos2d.h"
using namespace cocos2d;
struct Utils {

	static Rect getWorldRect(Node* item) {
		Size currentSize = Size(item->getContentSize().width * item->getScaleX(), item->getContentSize().height * item->getScaleY());
		Vec2 anchorPoint = item->getAnchorPoint();
		Vec2 offset = -(Vec2(anchorPoint.x * currentSize.width, anchorPoint.y * currentSize.height));
		Vec2 worldPosition = item->getParent()->convertToWorldSpace(item->getPosition() +offset);
		Node* currentNode = item;
		while ((currentNode = currentNode->getParent()) != nullptr) { // this works, gets the right size... Basically times the size by all the parent's scales
			currentSize.height *= currentNode->getScaleY();
			currentSize.width *= currentNode->getScaleX();
		}

		return Rect(worldPosition, currentSize);
	}
};