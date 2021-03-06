#ifndef __NetworkManager_h_
#define __NetworkManager_h_

#include <SDL/SDL_net.h>
#include "Singleton.h"
#include "GameServer.h"
#include "GameClient.h"
#include "GameState.h"

#include "HeartbeatPacket.h"
#include "VitalPacket.h"
#include "ParticlePacket.h"

#define DISCOVERY_PORT 9999
#define GAME_PORT 5555
#define HEARTBEATS_PER_SEC 20
#define HEARTBEAT_MAX_DELAY 1000.0f/HEARTBEATS_PER_SEC
#define DISCOVERY_SIGNATURE "JOEV"

struct PlayerIdInfo
{
	char type;
	uint32_t player_id;
	uint32_t current_map;
};

struct PlayerNumInfo
{
	char type;
	uint32_t num_player;
};

struct ServerAdvertisement
{
	char magic[5];
	char name[256]; // the hostname
	char description[256];
};

struct ChatPacket
{
	char type;
	char playerId;
	char message[256];
};

struct TeamIdPacket
{
	char type;
	char teamId;
};

struct PlayerJoinPacket
{
	char type;
	char playerId;
	char name[PLAYER_NAME_MAX_LEN];
	uint32_t game_mode;
	uint32_t team_mode;
	uint32_t current_map;
};

struct JoinRequestPacket
{
	char type;
	char name[PLAYER_NAME_MAX_LEN];
};

struct GameStartPacket
{
	char type;
	uint32_t game_mode;
	uint32_t team_mode;
	uint32_t current_map;
};

struct PlayerDisconnectPacket
{
	char type;
	char playerId;
};

struct PingPacket
{
	char type;
	char playerId;
};

struct GameOverPacket
{
	char type;
	char message[GAME_OVER_MSG_MAX_LEN];
};

enum NetworkManagerState 
{
	NetworkStateReady,
	NetworkStateClient,
	NetworkStateServer
};

class NetworkManager : public Singleton<NetworkManager> 
{
  public:

  	HeartbeatPacket* heartbeat;
	VitalPacket*     vital;
	ParticlePacket*  particle;

	uint32_t player_id;

	std::map<int, uint32_t> player_team_id_map;

  	NetworkManager();
  	~NetworkManager();

	NetworkManagerState state;

	void startServer();
	void stopServer();
	void startClient(const char* host);
	void stopClient();
	void startClientDiscovery();
	void stopClientDiscovery();
	void update();

	bool isActive();
	bool isServer();
	bool isClient();

	void sendChat(const char* msg);

	void receiveChat(ChatPacket* packet);
	void receiveHeartbeat(HeartBeatInfo* info);

	void changeId(uint32_t);

	// sends game state to every client
	void broadcastHeartbeat();
	void sendGameOverPacket();

	GameServer* server;
	GameClient* client;

	void send(void* data, int size, bool ack);

  private:

	// the timestamp on the last heartbeat
	boost::posix_time::ptime* _lastHeartbeat;
};

#endif
