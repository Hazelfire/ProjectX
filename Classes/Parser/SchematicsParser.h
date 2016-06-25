#include <unordered_map>
#include <vector>

typedef std::vector<std::vector<std::string>> StringTable;
struct Schematic {
	StringTable data[2];
	int width;
	int height;
};
typedef std::unordered_map<std::string, Schematic> SchematicMap;

class SchematicsParser {
public:
	static SchematicMap parse(std::string source);
};