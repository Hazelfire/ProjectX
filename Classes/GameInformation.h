#pragma once
#include <string>

struct PlayerInformation {

	// They are the same if they have the same name
	bool operator==(PlayerInformation other) { return (playerName == other.playerName); };

	std::string playerName;
	unsigned char classIndex;
};


struct GameInformation {
	PlayerInformation playerInfo;
	std::string mapName;
};

// Game modes that it is possible to play
enum class GameMode {
	SINGLEPLAYER,
	HOST_GAME,
	JOIN_GAME
};