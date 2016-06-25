#include <string>

enum DebugMessageType {
	DEBUG_ITEMS,
	DEBUG_GENERIC,
	DEBUG_CREATURES,
	DEBUG_MAPGEN,
	DEBUG_FONTS,
	DEBUG_IO,
	DEBUG_YAML,
	DEBUG_PLAYER_SELECTION,
	DEBUG_XML,
	DEBUG_MAP_SELECTION,
	DEBUG_LUA,
	DEBUG_INVENTORY
};

// A standard debugging tool, comes with logging of errors and information
class Debugger {
public:
	static void log(std::string message, DebugMessageType);
	static void logError(std::string error, DebugMessageType);
	static void logWarning(std::string warning, DebugMessageType);
};