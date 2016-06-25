#include <string>
#include <list>

using namespace std;
struct Requirement {
	string type;
	string region;
	list<Requirement> requirements;
	list<string> songs;
};

struct MusicData {
	list<Requirement> requirements;
};

class MusicParser {
public:
	static MusicData parse(string source);
private:
	struct Impl;
	static Impl impl;
};