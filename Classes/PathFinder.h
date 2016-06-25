#pragma once

#include "cocos2d.h"
#include <list>
#include <unordered_map>
#include "Arena.h"
USING_NS_CC;

#define HASHVEC2 int

struct PathNode {
	Vec2 location;
	std::stack < Vec2 > path;
};


class PathFinder {
public:
	static std::list<Vec2i> findPath(Vec2i start, Vec2i destination);
	static std::list<Vec2i> findPath(Vec2i start, Vec2i destination, int distance);

	static Sequence* findPathTo(Vec2 start, Vec2i destination, int distance, double speed);
	static Sequence* findPathTo(Vec2 start, Vec2i destination, int distance);
	static Sequence* findPathOn(Vec2 start, Vec2i destination);
private:
	static Sequence* findPathSequence(Vec2i start, Vec2i destination);
	static bool isPartOf(std::set<Vec2i> nodeList, Vec2i vector);
	static Vec2* sortVectors(Vec2i, Vec2i);
	static Vec2* sort(Vec2i*);
	static float guessDistance(Vec2i, Vec2i);
	static Vec2i smallestNode(std::set<Vec2i>, std::unordered_map<HASHVEC2,float > );
	static std::list<Vec2i> reconstructPath(std::unordered_map<HASHVEC2,Vec2i> , Vec2i);
	static std::list<Vec2i> findNeighbors(Vec2i, Vec2i);
};