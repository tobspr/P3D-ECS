
#include "enet_server_socket.h"

#include "enet_connected_peer.h"
#include "network_mutex.h"
#include "packet_logging.h"
#include "enet/enet.h"
#include <vector>

struct ENetServerSocketPy::impl {
  impl() {}
  ~impl() {
    for (ENetConnectedPeerPy* peer : _connected_peers)
      delete peer;
    if (_host)
      enet_host_destroy(_host);
  }

  bool start(size_t port, size_t max_connections, size_t max_channels, size_t max_incoming_bandwith,
             size_t max_outgoing_bandwith) {
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;

    _host = enet_host_create(&address, max_connections, max_connections, max_incoming_bandwith,
                             max_outgoing_bandwith);

    if (_host == nullptr) {
      std::cerr << "SERVER FAIL: Could not create enet host" << std::endl;
      return false;
    }

    return true;
  }

  ENetSocketEvent poll() {
    ENetEvent event;

    int ret = enet_host_service(_host, &event, 0);
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT: {
        char host_name[128];
        if (enet_address_get_host_ip(&event.peer->address, host_name, 127) != 0) {
          std::cerr << "FAILED to resolve new connection's host ip! Data: "
                    << event.peer->address.host << std::endl;
          memset(host_name, 0x00, 128);
        }

        std::cout << "A new client connected from " << host_name << ":" << event.peer->address.port
                  << std::endl;
        /* Store any relevant client information here. */

        ENetConnectedPeerPy* peer = new ENetConnectedPeerPy("server");
        peer->set_peer(event.peer);
        event.peer->data = peer;
        {
          NETWORK_THREAD_SAFE;
          _connected_peers.push_back(peer);
        }
        return ENetSocketEvent(peer, ENetSocketEvent::Type::Connect);
      }
      case ENET_EVENT_TYPE_RECEIVE:

        {
          ENetConnectedPeerPy* peer = static_cast<ENetConnectedPeerPy*>(event.peer->data);
          assert(peer);
          std::string bytes(event.packet->data, event.packet->data + event.packet->dataLength - 1);

          log_recv_packet("server", bytes, event.channelID);
          peer->on_message_recieved(bytes, event.channelID);
          enet_packet_destroy(event.packet);
          return ENetSocketEvent(nullptr, ENetSocketEvent::Type::MessageRecieved);
        }

      case ENET_EVENT_TYPE_DISCONNECT: {
        std::cout << event.peer->data << " disconnected." << std::endl;
        assert(event.peer->data);
        delete event.peer->data;
        event.peer->data = NULL;
        return ENetSocketEvent(nullptr, ENetSocketEvent::Type::Disconnect);
        break;
      }
    }

    return ENetSocketEvent(nullptr, ENetSocketEvent::Type::Empty);
  }

  size_t get_num_peers() const {
    NETWORK_THREAD_SAFE;
    return _connected_peers.size();
  }

  ENetConnectedPeerPy* get_peer(size_t i) {
    NETWORK_THREAD_SAFE;
    return _connected_peers[i];
  }

private:
  ENetHost* _host = nullptr;
  std::vector<ENetConnectedPeerPy*> _connected_peers;
};

ENetServerSocketPy::ENetServerSocketPy() {
  _impl = new impl;
}

ENetServerSocketPy::~ENetServerSocketPy() {
  delete _impl;
}

bool
ENetServerSocketPy::start(size_t port, size_t max_connections, size_t max_channels,
                          size_t max_incoming_bandwith, size_t max_outgoing_bandwith) {
  return _impl->start(port, max_connections, max_channels, max_incoming_bandwith,
                      max_outgoing_bandwith);
}

ENetSocketEvent
ENetServerSocketPy::poll() {
  return _impl->poll();
}

size_t
ENetServerSocketPy::get_num_peers() const {
  return _impl->get_num_peers();
}

ENetConnectedPeerPy*
ENetServerSocketPy::get_peer(size_t i) {
  return _impl->get_peer(i);
}
