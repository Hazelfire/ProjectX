/*
	This header file specifies the XProtocol, Specify new message types here.
	Currently supported types for sending:
	
	-  int
	- std::string
	- unsigned int
	- unsigned char
	- vec2i
	- vec2ull
	- double
	- float
	- char
*/
#pragma once

#include <vector>
#include <string>
#include <sstream>

#define MNULL '\x00'
#define MDELIMITER '\x01'

#include "XProtocolImpl.h"

// Connection, has 2 parameters
// 1: player Name
// 2: class Index
#define MCONNECT '\x02'
MESSAGE_TYPE_2(MCONNECT, std::string, unsigned char);

// Server can send currently connected players has 2 parameters:
// 1: player name
// 2: class index
#define MCONNECTED_PLAYER '\x04'
MESSAGE_TYPE_2(MCONNECTED_PLAYER, std::string, unsigned char);

// Tells a client what it's player index is, it has 1 argument
// 1: player index
#define MINDEX_ASIGN '\x05'
MESSAGE_TYPE_1(MINDEX_ASIGN, unsigned char);

// Tells the client that a player has disconnected
// 1: player index
#define MDISCONNECT '\x06'
MESSAGE_TYPE_1(MDISCONNECT, unsigned char);

// Gives information about the server connected
// 1: host's name
// 2: map name
#define MSERVER_INFO '\x07'
MESSAGE_TYPE_2(MSERVER_INFO, std::string, std::string);

// Sends the signal to start the game
// 1: the seed generated for the map
#define MSTART_GAME '\x08'
MESSAGE_TYPE_1(MSTART_GAME, unsigned int);

// Sends where the player spawned
// 1: the player's index
// 2: the player's coordinates
#define MSPAWNED '\x09'
MESSAGE_TYPE_2(MSPAWNED, unsigned char, Vec2i);

// Send that one player has moved to a position
// 1: the player's index
// 2: the player's coordinates
// 3: the player's distance
// 4: the player's speed
#define MPLAYER_MOVE_TO '\x0a'
MESSAGE_TYPE_4(MPLAYER_MOVE_TO, unsigned char,  Vec2i, unsigned char, float);

// Sends a change to the map
// 1: the tile's coordinates
// 2: the state of the tile
#define MMAP_STATE_CHANGE '\x0b'
MESSAGE_TYPE_2(MMAP_STATE_CHANGE, Vec2i, int);

// Informs a connecting player that the game has already started
#define MGAME_ALREADY_STARTED '\x0c'
MESSAGE_TYPE_0(MGAME_ALREADY_STARTED);

// Sends a change to a tile property
// 1: the tile's coordinates
// 2: the property being changed
// 3: the value of the property
#define MMAP_PROPERTY_CHANGE '\x0d'
MESSAGE_TYPE_3(MMAP_PROPERTY_CHANGE, Vec2i, std::string, int);

// A creature has moved to a position
// 1: the creatures index
// 2: the creature's coordinates
// 3: the distance
// 4: the speed
#define MCREATURE_MOVE_TO '\x0e'
MESSAGE_TYPE_4(MCREATURE_MOVE_TO, unsigned int, Vec2i, unsigned char, float);

// A creature has spawned
// 1: the creatures name
// 2: the coordinates of the creature
#define MCREATURE_SPAWN '\x0f'
MESSAGE_TYPE_2(MCREATURE_SPAWN, std::string, Vec2i);

// Gives a player an item
// 1: the player's index
// 2: the item's name
// 3: the quantity
#define MGIVE_PLAYER_ITEM '\x10'
MESSAGE_TYPE_3(MGIVE_PLAYER_ITEM, unsigned char, std::string, unsigned int);

// Takes items from the player
// 1: the player's index
// 2: the item's name
// 3: the quantity
#define MTAKE_ITEM '\x11'
MESSAGE_TYPE_3(MTAKE_ITEM, unsigned char, std::string, unsigned int);

// Sends a change to tile properties
// 1: the coordinates of the tile
// 2: the properties of the tile
#define MSET_TILE_PROPERTIES '\x12'
MESSAGE_TYPE_2(MSET_TILE_PROPERTIES, Vec2i, std::string);


// Sends the hash of the resources directory
// 1: the hash itself
// 2: the directory of the hash
#define MSOURCE_HASH '\x13'
MESSAGE_TYPE_2(MSOURCE_HASH, unsigned int, std::string);

// Asks for the hash of a certain directory
// 1: the directory
#define MHASH_REQUEST '\x14'
MESSAGE_TYPE_1(MHASH_REQUEST, std::string);

// Sends a change to a creature's properties

// Sends a change to creature movement speed

// Sends a change to player movement speed
