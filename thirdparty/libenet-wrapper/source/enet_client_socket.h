#pragma once
#ifndef ENET_CLIENT_SOCKET_H
#define ENET_CLIENT_SOCKET_H

#include "config_libenetwrap.h"
#include "enet_socket_event.h"
#include <string>

class ENetConnectedPeerPy;

class ENetClientSocketPy {
  PUBLISHED : ENetClientSocketPy();
  ~ENetClientSocketPy();

  ENetConnectedPeerPy* connect(const std::string& address, size_t port, size_t max_channels = 2,
                               size_t max_outgoing_bandwith = 57600,
                               size_t max_incoming_bandwith = 14400, size_t timeout = 5000);
  ENetSocketEvent poll();

public:
  struct impl;

private:
  impl* _impl;
};

#endif
