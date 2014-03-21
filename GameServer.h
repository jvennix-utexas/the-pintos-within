#ifndef __GameServer_h_
#define __GameServer_h_

#include <SDL/SDL_net.h>
#include "common.h"

enum GameServerStatus {
  GameServerReady,
  GameServerRunning,
  GameServerStopped
};

class GameServer {

public:

  GameServer(int port);

  // Starts a UDP server in a bg thread
  int start();

  // Stops the bg thread (eventually)
  void stop();

  // the state of the server
  GameServerStatus state;

  // called once per game loop
  void update();

  // sends game state to every client
  void sendHeartbeat();

private:

  // the socket that is bound
  UDPsocket _socket;

  // the UDP port
  int _port;

  // list of connected clients (addresses)
  std::vector<IPaddress> _clients;

  // nom nom all the packets off the wire
  void consumePackets();

  // logic of single packet processing
  void processPacket(UDPpacket* packet);

  // handles saving client and sending ACK
  void handleJoinPacket(UDPpacket* packet);

  // sends a single packet back to a single client
  void sendPacketToClient(UDPpacket* packet, IPaddress* ip);

  // broadcasts a single packet to all connected clients
  void broadcastPacket(UDPpacket* packet);

  // a temporarily allocated packet for sending on the wire
  UDPpacket *_tmpSendPacket;

  // a temporarily allocated packet for consuming the wire
  UDPpacket *_tmpRecvPacket;
};

#endif
