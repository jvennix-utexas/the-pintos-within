#ifndef __NetworkManager_h_
#define __NetworkManager_h_

#include <SDL/SDL_net.h>
#include "Singleton.h"
#include "GameServer.h"
#include "GameClient.h"
#include "GameState.h"
#include "GUIManager.h"

#include "HeartbeatPacket.h"
#include "VitalPacket.h"
#include "ParticlePacket.h"

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
	VitalPacket* vital;
	ParticlePacket* particle;

	uint32_t player_id;

  	NetworkManager();
  	~NetworkManager();

	NetworkManagerState state;

	void startServer();
	void startClient(char* host);
	void update();

	bool isActive();
	bool isServer();
	bool isClient();

	void sendHeartbeat();
	void sendVital();
	void sendParticle();

	void receiveHeartbeat(HeartBeatInfo* info);
	void receiveVital(VitalInfo* info);
	void receiveParticle(ParticleInfo* info);

	void changeId(uint32_t);

	GameServer* server;
	GameClient* client;
};

#endif
