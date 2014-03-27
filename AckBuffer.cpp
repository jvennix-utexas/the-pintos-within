#include "AckBuffer.h"

namespace pt = boost::posix_time;

Ack::Ack(IPaddress addr, AckId id, void* packetData, int packetLen) {
  this->address    = addr;
  this->id         = id;
  this->packetLen  = packetLen-sizeof(AckPacket);
  this->packetData = malloc(packetLen);
  memcpy(this->packetData, packetData+sizeof(AckPacket), packetLen);
  reset();
}

Ack::~Ack() {
  free(this->packetData);
}

bool Ack::isExpired() {
  pt::ptime now          = pt::microsec_clock::local_time();
  pt::time_duration diff = now - *sentAt;
  return diff.total_milliseconds() > ACK_EXPIRATION_MS;
}

void Ack::reset() {
  pt::ptime now = pt::microsec_clock::local_time();
  this->sentAt     =  (pt::ptime*)malloc(sizeof(pt::ptime));
  memcpy(sentAt, &now, sizeof(now));
}

AckBuffer::AckBuffer() {
  this->currentId = 0;
}

bool AckBuffer::hasAckId(AckId id) {
  std::map<AckId, Ack*>::iterator it = buffer.find(id);
  return it != buffer.end();
}

AckId AckBuffer::injectAck(UDPpacket* packet, IPaddress addr) {
  Ack *a = new Ack(addr, currentId++, packet->data, packet->len);
  buffer[a->id] = a;
  return a->id;
}

void AckBuffer::forgetAck(AckId id) {
  if (hasAckId(id)) {
    delete buffer[id];
    buffer.erase(id);
  }
}