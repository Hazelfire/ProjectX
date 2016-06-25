#include <unordered_map>
#include <list>
using namespace std;

struct Interaction {
	string name;
	string command;
	string ifConditional;
	bool instant = false;
};
struct ActionList{
	bool isMovable = false;
	list < Interaction > options;
};

struct ActionMap {
	std::unordered_map<std::string, ActionList> tiles;
	std::unordered_map<std::string, ActionList> creatures;
};

class InteractionsParser {
public:
	static ActionMap parse(string source);
private:
	struct Impl;
	static Impl impl;
};