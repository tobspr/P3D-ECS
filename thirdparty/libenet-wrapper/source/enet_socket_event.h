#pragma once

#ifndef ENET_SOCKET_EVENT_H
#define ENET_SOCKET_EVENT_H

#include "config_libenetwrap.h"
#include <string>

class ENetConnectedPeerPy;

class ENetSocketEvent {
  PUBLISHED : enum Type { Empty, Connect, Disconnect, MessageRecieved };

public:
  explicit ENetSocketEvent(ENetConnectedPeerPy* peer, Type type)
    : _peer(peer)
    , _type(type){};

PUBLISHED:
  inline ENetConnectedPeerPy* get_peer() { return _peer; }
  inline Type get_type() { return _type; }

  MAKE_PROPERTY(peer, get_peer);
  MAKE_PROPERTY(type, get_type);

private:
  ENetConnectedPeerPy* _peer;
  Type _type;
};



#endif