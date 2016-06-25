#include "MultiplayerClient.h"
#include <functional>
#include "GameInformation.h"
#include <list>
#include "MyMath.h"

// Client with functionality specific to the game
class XClient : public MultiplayerClient {
public:
	
	// an interface for the lobby
	class Lobby{
	public:
		virtual void onPlayerConnect(PlayerInformation playerInfo) = 0;
		virtual void onPlayerDisconnect(int playerIndex) = 0;
		virtual void onServerInformation(std::string hostName, std::string map) = 0;
		virtual void onError(std::string error) = 0;

		typedef std::list<PlayerInformation> PlayerList;
		PlayerList m_connectedPlayers;
	};

	// stores chosen information about the player
	void init(PlayerInformation gameInfo, Lobby* lobby);

	// gets the current player's index
	static int getPlayerIndex();

	static XClient* getInstance();

	// starts the game
	void startGame();

	// Sends information that the player has moved
	void movePlayer(Vec2i position, unsigned char distance, float speed);

	void moveCreature(int creatureIndex, Vec2i position, int distance, float speed);

	// Sends information that the map has changed
	void changeMap(Vec2i position, int state);

	// Send information that a creature has spawned
	void spawnCreature(std::string creatureName, Vec2i location);

	// Sends an item over
	void giveItem(unsigned char playerIndex, std::string itemName, unsigned int quantity);

	// Takes an item
	void takeItem(unsigned char playerIndex, std::string itemName, unsigned int quantity);

	// Sets tile properties
	void setTileProperties(Vec2i tileLocation, std::string properties);
private:

	static XClient m_clientInstance;

	Lobby* m_lobby;

	bool m_isGameState = false;

	// Client events
	virtual void onConnect();
	virtual void onMessage(std::string& data);
	virtual void onClose();
	virtual void onError(std::string error);

	PlayerInformation m_playerInfo;

	std::string m_mapName;

	static int m_playerIndex;
};