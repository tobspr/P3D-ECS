#pragma once

#ifndef ENET_CONNECTED_PEER_H
#define ENET_CONNECTED_PEER_H

#include "config_libenetwrap.h"
#include <string>

// Needs a bit different forward definiton because thats how enet declares its types
struct _ENetPeer;
typedef _ENetPeer ENetPeer;

#ifdef INTERROGATE
typedef unsigned char enet_uint8;
#else
#include "enet/types.h"
#endif

class ENetConnectedPeerPy {
  PUBLISHED : ENetConnectedPeerPy(const std::string& context);
  ~ENetConnectedPeerPy();

  bool get_has_messages() const;
  MAKE_PROPERTY(has_messages, get_has_messages);
  std::string take_message();

  void send_message(const std::string& bytes, enet_uint8 channel = 0, bool reliable = true);

public:
#ifndef INTERROGATE
  void set_peer(ENetPeer* peer);
  ENetPeer* get_peer();
  void on_message_recieved(const std::string& bytes, enet_uint8 channel);
#endif

public:
  struct impl;
  inline impl* get_impl() { return _impl; };

private:
  impl* _impl;
};

#endif
