#pragma once
#ifndef ENET_SERVER_SOCKET_H
#define ENET_SERVER_SOCKET_H

#include "enet_socket_event.h"

class ENetServerSocketPy {
  PUBLISHED :

    ENetServerSocketPy();
  ~ENetServerSocketPy();

  bool start(size_t port, size_t max_connections = 32, size_t max_channels = 2,
             size_t max_incoming_bandwith = 0, size_t max_outgoing_bandwith = 0);

  ENetSocketEvent poll();

  size_t get_num_peers() const;
  ENetConnectedPeerPy* get_peer(size_t i);

  MAKE_PROPERTY(num_peers, get_num_peers);
  
  // Leaks memory!
  //MAKE_SEQ_PROPERTY(connected_peers, get_num_peers, get_peer);

public:
  // Pimpl to make sure interrogate can parse this
  struct impl;

private:
  impl* _impl;
};

#endif