#include "PathFinder.h"
#include "Arena.h"
#include "Interactions.h"
#include "Parser/StringOperations.h"

#define PATHFINDING_LIMIT 50
using namespace std;
/*
This algorithm is A* pathfinding, Look it up on wikipedia. I copied the source code from the psuedocode example.
The key part of the algorithm is the guessDistance function, which can be changed to achieve different effects. If the function underestimes
the length then it will be faster but not find the shortest root, if it overestimates it will always find the shortest path
*/


// The difference in this new algorithm is that this one can take moves off the end of its path, meaning that a player can move Towards
// an object and not only onto it (distance 0 is the same as the old code, as the overload suggest below)
Sequence* PathFinder::findPathTo(Vec2 start, Vec2i destination, int distance, double speed) {
#define CENTER_PLAYER start.equals(flooredStart)? nullptr: Sequence::create(MoveTo::create(start.distance(flooredStart)/ speed, XTileMap::findWorldLocationAt(flooredStart)), NULL)
	Vec2 flooredStart = Vec2((int)start.x, (int)start.y);
	if (flooredStart.equals(destination)) {
		return CENTER_PLAYER;
	}
	list<Vec2i> path = findPath(start, destination);
	if (path.empty()) return CENTER_PLAYER;
	Vec2 lastTile = start;
	Vector<FiniteTimeAction*> actions;
	actions.reserve(path.size());

	if (path.empty()) return CENTER_PLAYER;
	int i = 0;
	while (i < distance) {
		path.pop_back(); // makes sure that he wont make it to the destination (hence TO int findPath To)
		if (path.empty()) return CENTER_PLAYER;
		i++;
	}
	int index = 0;
	for (list<Vec2i>::iterator currentNode = path.begin(); currentNode != path.end(); currentNode++) {
		actions.insert(index, MoveTo::create(currentNode->distance(lastTile)  / speed, XTileMap::findWorldLocationAt(*currentNode)));
		index++;
		lastTile = *currentNode;
	}
	return Sequence::create(actions);
}

Sequence* PathFinder::findPathTo(Vec2 start, Vec2i destination, int distance) {
	return findPathTo(start, destination,distance, Player::getInstance()->getMovementSpeed());
}

Sequence* PathFinder::findPathOn(Vec2 start, Vec2i destination) {
	return findPathTo(start, destination, 0);
}

// Another overload for find path, takes a distance
std::list<Vec2i> PathFinder::findPath(Vec2i start, Vec2i destination, int distance) {
	std::list<Vec2i> path = findPath(start, destination);
	for (int i = 0; i < distance; i++) {
		if (!path.empty()) {
			path.pop_back();
			if (path.empty()) {
				return path;
			}
		}
	}
	return path;
}


list<Vec2i> PathFinder::findPath(Vec2i start, Vec2i destination) {
	set<Vec2i> closedSet;
	set<Vec2i> openSet;
	openSet.insert(start);
	unordered_map<HASHVEC2, Vec2i> cameFrom;
	unordered_map<HASHVEC2, float> gScore;
	gScore[start.hash()] = 0;

	unordered_map<HASHVEC2, float> fScore;
	fScore[start.hash()] = guessDistance(start,destination);
	int i = 0;
	while (!openSet.empty() && i< PATHFINDING_LIMIT) {
		Vec2i current = smallestNode(openSet,fScore);
		if (current == destination) {
			list<Vec2i> path = reconstructPath(cameFrom, destination);
			path.pop_front(); // gets rid if the tile the player is already on
			return path;
		}
		openSet.erase(current);
		closedSet.insert(current);
		list<Vec2i> neighbors = findNeighbors(current, destination);
		for (list<Vec2i>::iterator currentNeighbor = neighbors.begin(); currentNeighbor != neighbors.end(); currentNeighbor++) {
			if (closedSet.find(*currentNeighbor) != closedSet.end()) continue; // Ignore if it has already been evaluated
			float tentatuve_gScore = gScore[current.hash()] + current.distance(*currentNeighbor);
			if (openSet.find(*currentNeighbor) == openSet.end()) {
				openSet.insert(*currentNeighbor);
			}
			else if (tentatuve_gScore >= gScore[currentNeighbor->hash()]) continue; // Not a better path to open node
			cameFrom[currentNeighbor->hash()] = current;
			gScore[currentNeighbor->hash()] = tentatuve_gScore;
			fScore[currentNeighbor->hash()] = tentatuve_gScore + guessDistance(*currentNeighbor, destination);
		}
		i++;
	}
	// Path not found
	return list<Vec2i>();
}

float PathFinder::guessDistance(Vec2i oneTile, Vec2i otherTile) {
	return oneTile.distanceSquared(otherTile);
}
Vec2i PathFinder::smallestNode(set<Vec2i> VecSet, unordered_map<HASHVEC2, float > scoreMap) {
	Vec2 smallestNode;
	int smallestValue = 10000;
	for (set<Vec2i>::iterator currentItem = VecSet.begin(); currentItem != VecSet.end(); currentItem++) {
		if (scoreMap[currentItem->hash()] < smallestValue) {
			smallestValue = scoreMap[currentItem->hash()];
			smallestNode = *currentItem;
		}
	}
	return smallestNode;
}
list<Vec2i> PathFinder::reconstructPath(unordered_map<HASHVEC2, Vec2i> cameFrom, Vec2i goal) {
	list<Vec2i> re;
	Vec2i currentNode = goal;
	re.push_front(currentNode);
	int i = 0;
	while (cameFrom.find(currentNode.hash())!= cameFrom.end()) {
		if (i > 50) {
			return list<Vec2i>(); // Sometimes there are infinite pathfinding loops... not sure how to duplicate though
		}
		Vec2 nextNode = cameFrom[currentNode.hash()];
		re.push_front(nextNode);
		currentNode = nextNode;
		i++;
	}
	return re;
}
list<Vec2i> PathFinder::findNeighbors(Vec2i tile, Vec2i destination) {
	static const Vec2i neighbors[] = {Vec2i(-1,-1), Vec2i(0,-1), Vec2i(1,-1), Vec2i(-1,0), Vec2i(1,0), Vec2i(-1,1), Vec2i(0,1), Vec2i(1,1)};
	list<Vec2i> re;
	Vec2i flooredTile = Vec2i((int)tile.x, (int)tile.y);

	for (int i = 0; i < 8; i++) {
		if(Arena::getMapInstance()->isMovable(flooredTile + neighbors[i]) || (flooredTile + neighbors[i]).equals(destination)){
		re.push_front(flooredTile + neighbors[i]);
		}
	}
	return re;
}
