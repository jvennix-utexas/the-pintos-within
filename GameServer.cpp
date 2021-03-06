#include "GameServer.h"
#include "GameState.h"
#include "NetworkManager.h"
#include "HeartbeatPacket.h"
#include "VitalPacket.h"
#include "ParticlePacket.h"
#include "GuiManager.h"

namespace pt = boost::posix_time;

GameServer::GameServer(int port) 
{
	state          = GameServerReady;
	_port          = port;
	_tmpSendPacket = SDLNet_AllocPacket(4096);
	_tmpRecvPacket = SDLNet_AllocPacket(4096);
	_ackBuffer     = new AckBuffer();
	_hostname      = NULL;
	_multicastDebouncer = new Debouncer(1000, []() {
		if (!GameState::instance()->isRunning()) {
			NetworkManager::instance()->server->sendAdvertisement();
		}
	});
	// every 3 seconds
	_pingDebouncer = new Debouncer(PING_INTERVAL, []() {
		char c = PING;
		NetworkManager::instance()->server->broadcastData(&c, 1, false);
		NetworkManager::instance()->server->bootInactivePlayers();
	});
}

GameServer::~GameServer() {
	if (_socket) {
		SDLNet_UDP_Close(_socket);
		_socket = NULL;
	}
	SDLNet_FreePacket(_tmpSendPacket);
	SDLNet_FreePacket(_tmpRecvPacket);
	delete _ackBuffer;
	delete _multicastDebouncer;
	delete _pingDebouncer;
	if (_hostname) free(_hostname);
}

// starts the web server (opens UDP port 5555)
int GameServer::start() 
{
	GameState::instance()->num_player = 1;

	if (state != GameServerReady) return -1;

	NetworkManager::instance()->changeId(0);

	state = GameServerRunning;

	if (!(_socket = SDLNet_UDP_Open(_port))) {
		fprintf(stderr, "SDLNet_UDP_Open: %s\n", SDLNet_GetError());
		return -1;
	}

	if (SDLNet_ResolveHost(&_udpBroadcastAddress, "255.255.255.255", DISCOVERY_PORT) == -1) {
		fprintf(stderr, "Failed to resolve broadcast address: %s\n", SDLNet_GetError());
		return -1;
	}

	_hostname = (char*)malloc(256);
	gethostname(_hostname, 256);

	return 0;
}

// stops the web server
void GameServer::stop() 
{
	if (state != GameServerRunning) return;
	state = GameServerStopped;
}

// called from the render loop
void GameServer::update() 
{
	consumePackets();
	_multicastDebouncer->run();
	_pingDebouncer->run();
}

// sends a single packet to a single client
void GameServer::sendPacketToClient(UDPpacket* packet, IPaddress* ip) {
	// printf("Sending response packet to %x:%x..\n", ip->host, ip->port);

	// unbind from our previous client
	SDLNet_UDP_Unbind(_socket, 0);

	// this is kinda dumb. i bind to a new channel on every send
	// because i am too lazy to do manual packet address fixup :)

	if (SDLNet_UDP_Bind(_socket, 0, ip) < 0) {
		fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
	}

	if (SDLNet_UDP_Send(_socket, 0, _tmpSendPacket) < 0) {
		fprintf(stderr, "SDLNet_UDP_Send: %s\n", SDLNet_GetError());
	}
}


  // this puts an AckHeader struct above the data in the packet, and adds
  // the request to the ack buffer if necessary.
void GameServer::putDataIntoPacket(UDPpacket* p, void* data, int len, IPaddress* ip,
									bool ack, AckId id, bool isResponse) {
	// copy the data after the ack packet
	memcpy(p->data+MEMALIGNED_SIZE(AckHeader), data, len);
	p->len = len+MEMALIGNED_SIZE(AckHeader);

	// we will be shoving our ACK on top like a baller
	AckHeader ackPack;
	ackPack.ackRequired = ack;
	ackPack.isResponse  = isResponse;
	// inject the ACK info if necessary
	if (ack && id == 0) {
		ackPack.id = _ackBuffer->injectAck(p, *ip);
	} else {
		ackPack.id = id;
	}
	// shove the ACK on top!
	memcpy(p->data, &ackPack, MEMALIGNED_SIZE(AckHeader));
}

// Adds the ACK header and sends to client
void GameServer::sendDataToClient(void* data, int len, IPaddress* ip, bool ack,
								  AckId id, bool isResponse) {
	putDataIntoPacket(_tmpSendPacket, data, len, ip, ack, id, isResponse);
	sendPacketToClient(_tmpSendPacket, ip);
}

// shells out to the above implementation
void GameServer::sendDataToClient(void* data, int len, int cliId, bool ack,
								  AckId id, bool isResponse) {
	if (cliId <= 0) return;
	IPaddress ip = _clients.at(cliId-1);
	sendDataToClient(data, len, &ip, ack, id, isResponse);
}


// broadcasts a single chunk of data to a bunch of clients
// this method can be used for binary or cstring (NULL terminated) buffer
void GameServer::broadcastData(void* data, int len, bool ack) {
	for (int i = 0; i < _clients.size(); i++) {
		IPaddress ip = _clients.at(i);
		sendDataToClient(data, len, &ip, ack);
	}
}

void GameServer::resendExpiredAcks() {
	std::map<AckId, Ack*>::iterator iter;
	for (iter = _ackBuffer->buffer.begin(); iter != _ackBuffer->buffer.end();) {
		Ack* ack = iter->second;
		if (ack->isExpired()) {
			// LOG("ACK EXPIRED. RESENDING REQUEST.");
			sendDataToClient(ack->packetData, ack->packetLen, &ack->address, true, ack->id);
			ack->reset();
		} else {
			iter++;
		}
	}
}

// nom noms any available UDP packets from the wire
void GameServer::consumePackets() {
	if (state != GameServerRunning) {
		return;
	}

	// Consume all available packets in the buffer
	while (SDLNet_UDP_Recv(_socket, _tmpRecvPacket)) {
		processPacket(_tmpRecvPacket);
	}
}

// sends a UDP broadcast to clients in the same NAT subnet
// that we are running a LAN game here.
void GameServer::sendAdvertisement() {
	LOG("BROADCASTING ADVERTISEMENT");
	ServerAdvertisement ad;
	strncpy(ad.magic, DISCOVERY_SIGNATURE, 5);
	strncpy(ad.name, _hostname, 256);
	strncpy(ad.description, GameState::instance()->GetGameDescription().c_str(), 256);
	sendDataToClient(&ad, sizeof(ServerAdvertisement), &_udpBroadcastAddress, false);
}

void GameServer::broadcastGameStart() 
{
	GameStartPacket start_packet;
	start_packet.type = GAMESTART;
	start_packet.game_mode = GameState::instance()->game_mode;
	start_packet.team_mode = GameState::instance()->team_mode;
	start_packet.current_map = GameState::instance()->current_map;
	broadcastData(&start_packet, sizeof(GameStartPacket), true);
}

void GameServer::bootInactivePlayers() 
{
	pt::ptime now = pt::second_clock::local_time();
	for(std::map<int,bool>::iterator iter = GameState::instance()->playerConnections.begin();
            iter != GameState::instance()->playerConnections.end(); ++iter)
      {
        int i = iter->first; // the server always be active yo
        if (i == 0) continue;
        pt::time_duration diff = now - _lastReceived[i];
        if (diff.total_seconds() > CLIENT_TIMEOUT) 
        {
        	LOG("CLIENT " << i << " TIMED OUT... DISCONNECTING THIS CLIENT");

        	PlayerDisconnectPacket p;
        	p.type = PLAYER_DISCONNECT;
        	p.playerId = i;

        	if((GameState::instance()->players[i] != NULL && GameState::instance()->players[i]->in_pinto_form
        		&& !GameState::instance()->players[i]->is_dead) ||
        		(GameState::instance()->player_pinto_seeds[i]))
			{
				uint random_pinto_index;
				do
				{
					random_pinto_index = RAND_RANGE(0, GameState::instance()->num_player);

					int loop = 0;
					for(std::map<int,bool>::iterator iter = GameState::instance()->playerConnections.begin();
			            iter != GameState::instance()->playerConnections.end(); ++iter) {
						if (loop == random_pinto_index) { random_pinto_index = iter->first; break; }
						loop++;
					}
				}while(random_pinto_index == i);

				if(GameState::instance()->players[random_pinto_index] == NULL
					|| (GameState::instance()->players[random_pinto_index]->is_dead))
				{
					GameState::instance()->player_pinto_seeds[random_pinto_index] = true;
				}
				else
				{
					GameState::instance()->players[random_pinto_index]->changeToPinto();
				}
				NetworkManager::instance()->vital->setChangePinto(random_pinto_index);
			}

        	broadcastData(&p, sizeof(PlayerDisconnectPacket), true);

			GameState::instance()->removePlayer(i);
        }
	}
}

// This is the "meat" of the packet processing logic in GameServer
// Requests are dished out based on their first byte
void GameServer::processPacket(UDPpacket* packet) {

	AckHeader* ackHeader = (AckHeader*)packet->data;
	void* packetData = packet->data+MEMALIGNED_SIZE(AckHeader);
	char packetType = ((char*)packetData)[0];
	// printf("PacketType: %c\n", packetType);

	if (ackHeader->isResponse) {
		// woot. expire our ACK.
		_ackBuffer->forgetAck(ackHeader->id);
		// LOG("ACK RECEIVED BY HOST: "<<ackHeader->id);
		return;
	} else if (ackHeader->ackRequired) {
		// fire off the ACK!
		sendDataToClient((void*)"A", 2, &(packet->address), false, ackHeader->id, true);
		// LOG("ACK REPLIED BY HOST: "<<ackHeader->id);
	}

	switch (packetType) {
		case JOINGAME:
			// JOIN request adds a character to the game
			handleJoinPacket(packet, packetData);
			break;
		case HEARTBEATPACK:
			HeartBeatInfo* hinfo;
			hinfo =  (HeartBeatInfo*) packetData;
			NetworkManager::instance()->receiveHeartbeat(hinfo);
			broadcastData(hinfo, sizeof(HeartBeatInfo), false);
			break;
		case CHATPACK:
			ChatPacket* chat;
			chat = (ChatPacket*)packetData;
			NetworkManager::instance()->receiveChat(chat);
			broadcastData(chat, sizeof(ChatPacket), true);
			break;
		case TAKEDAMAGE:
			PlayerDamageInfo* damage_info;
			damage_info =  (PlayerDamageInfo*) packetData;
			NetworkManager::instance()->vital->receiveDamage(damage_info);
			broadcastData(damage_info, sizeof(PlayerDamageInfo), true);
			break;
		case PLAYER_DIE:
			PlayerDieInfo* player_die_info;
			player_die_info =  (PlayerDieInfo*) packetData;
			NetworkManager::instance()->vital->receivePlayerDie(player_die_info);
			break;
		case WEAPON_CHANGE:
			ChangeWeaponInfo* weapon_change_info;
			weapon_change_info =  (ChangeWeaponInfo*) packetData;
			NetworkManager::instance()->vital->receiveChangeWeapon(weapon_change_info);
			broadcastData(weapon_change_info, sizeof(ChangeWeaponInfo), true);
			break;
		case WEAPON_SPAWN:
			WeaponSpawnInfo* weapon_spawn_info;
			weapon_spawn_info =  (WeaponSpawnInfo*) packetData;
			NetworkManager::instance()->vital->receiveSpawnWeapon(weapon_spawn_info);
			broadcastData(weapon_spawn_info, sizeof(WeaponSpawnInfo), true);
			break;
		case BLOOD:
			BloodInfo* blood_info;
			blood_info =  (BloodInfo*) packetData;
			NetworkManager::instance()->particle->receiveBlood(blood_info);
			broadcastData(blood_info, sizeof(BloodInfo), true);
			break;
		case DUST:
			DustInfo* dust_info;
			dust_info =  (DustInfo*) packetData;
			NetworkManager::instance()->particle->receiveDust(dust_info);
			broadcastData(dust_info, sizeof(DustInfo), true);
			break;
		case BLASTER_EXPLODE:
			BlasterExplodeInfo* blaster_info;
			blaster_info =  (BlasterExplodeInfo*) packetData;
			NetworkManager::instance()->particle->receiveBlasterExplosion(blaster_info);
			broadcastData(blaster_info, sizeof(BlasterExplodeInfo), true);
			break;
		case PLAY_FIRE_SOUND:
			PlayFireSoundInfo* fire_info;
			fire_info =  (PlayFireSoundInfo*) packetData;
			NetworkManager::instance()->vital->receivePlayFireSound(fire_info);
			broadcastData(fire_info, sizeof(PlayFireSoundInfo), true);
			break;
		case CHANGE_PINTO:
			ChangePintoInfo* change_pinto_info;
			change_pinto_info =  (ChangePintoInfo*) packetData;
			NetworkManager::instance()->vital->receiveChangePinto(change_pinto_info);
			broadcastData(change_pinto_info, sizeof(ChangePintoInfo), true);
			break;
		case INCREASE_SCORE:
			LOG("INCREASE_SCORE PACKET FOUND!");
			IncreaseScoreInfo* score_info;
			score_info =  (IncreaseScoreInfo*) packetData;
			NetworkManager::instance()->vital->receiveIncreaseScore(score_info);
			broadcastData(score_info, sizeof(IncreaseScoreInfo), true);
			break;
		case PING:
			PingPacket *p;
			p = (PingPacket*) packetData;
			_lastReceived[p->playerId] = boost::posix_time::second_clock::local_time();
			break;
		case HAIR_CHANGE:
			ChangeHairInfo *hair;
			hair =  (ChangeHairInfo*) packetData;
			NetworkManager::instance()->vital->receiveChangeHair(hair);
			broadcastData(hair, sizeof(ChangeHairInfo), true);
			break;
	}
}

// when a client joins, we need to ACK back that it succeeded
void GameServer::handleJoinPacket(UDPpacket *packet, void* data) {
	IPaddress ip;

	printf("CLIENT %x %d JOINED\n", packet->address.host, packet->address.port);

	JoinRequestPacket* join;
	join = (JoinRequestPacket*)data;
	LOG("PLAYER'S NAME IS " << join->name);

	_clients.push_back(packet->address);
	int id = _clients.size();
	GameState::instance()->num_player++;

	_lastReceived[id] = boost::posix_time::second_clock::local_time();

	int len = strlen(join->name);
	if (len > 14) len = 14;
	char c = '1';
	while (GameState::instance()->nameIsTaken(join->name)) {
		LOG("NAME IS TAKEN!!!!");
		join->name[len] = c++;
		join->name[len+1] = '\0';
		LOG("NEW NAME= " << join->name);
	}

	GameState::instance()->setPlayerName(id, std::string(join->name));

	memcpy(&ip, &(packet->address), sizeof(IPaddress));

	PlayerIdInfo info;
	info.type = ASSIGNPLAYERID;
	info.player_id = id;
	info.current_map = GameState::instance()->current_map;

	char x = ASSIGNPLAYERID;
	sendDataToClient(&info, sizeof(PlayerIdInfo), &ip, true);

	// send a PlayerJoinPacket to the new client for every other player
	for(std::map<int,bool>::iterator iter = GameState::instance()->playerConnections.begin();
		iter != GameState::instance()->playerConnections.end(); ++iter) {
		int i = iter->first;
		PlayerJoinPacket p;
		p.type = PLAYER_JOIN;
		p.playerId = i;
		p.game_mode = GameState::instance()->game_mode;
		p.team_mode = GameState::instance()->team_mode;
		p.current_map = GameState::instance()->current_map;

		strncpy(p.name, GameState::instance()->getPlayerName(i).c_str(), PLAYER_NAME_MAX_LEN);
		p.name[PLAYER_NAME_MAX_LEN-1] = '\0';
		sendDataToClient(&p, sizeof(PlayerJoinPacket), &ip, true);
	}

	// broadcasts a PlayerJoinPacket to the other clients about the new client
	PlayerJoinPacket p;
	p.type = PLAYER_JOIN;
	p.playerId = id;
	strncpy(p.name, join->name, PLAYER_NAME_MAX_LEN);
	p.name[PLAYER_NAME_MAX_LEN-1] = '\0';
	broadcastData(&p, sizeof(PlayerJoinPacket), true);

	LOG("Assigning player id: " << id);
        GuiManager::instance()->EnableStart();
}
