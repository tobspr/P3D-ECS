
#include "enet_client_socket.h"

#include "enet/enet.h"

#include "enet_connected_peer.h"
#include "packet_logging.h"
#include <string>

struct ENetClientSocketPy::impl {

  impl(){};
  ~impl() {
    if (_connected_peer)
      delete _connected_peer;
    if (_host)
      enet_host_destroy(_host);
  };

  ENetConnectedPeerPy* connect(const std::string& host, size_t port, size_t max_channels,
                               size_t max_outgoing_bandwith, size_t max_incoming_bandwith,
                               size_t timeout) {

    _host = enet_host_create(NULL, 1, max_channels, max_incoming_bandwith, max_outgoing_bandwith);
    if (_host == nullptr) {
      std::cerr << "RECV: FAILED to create client host." << std::endl;
      return nullptr;
    }

    ENetAddress address;
    enet_address_set_host(&address, host.c_str());
    address.port = port;

    ENetPeer* peer = enet_host_connect(_host, &address, max_channels, 0);
    if (peer == nullptr) {
      std::cerr << "RECV: FAILED to create peer for connection." << std::endl;
      return nullptr;
    }

    // Actual connection attempt
    ENetEvent event;
    if (enet_host_service(_host, &event, timeout) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
      std::cout << "RECV: Connection to " << host << ":" << port << " succeeded." << std::endl;
      _connected_peer = new ENetConnectedPeerPy();
      _connected_peer->set_peer(peer);
      _connected_peer->set_host(_host);

    } else {
      /* Either the 5 seconds are up or a disconnect event was */
      /* received. Reset the peer in the event the 5 seconds   */
      /* had run out without any significant event.            */
      enet_peer_reset(peer);
      std::cout << "RECV: Connection to " << host << ":" << port << " failed." << std::endl;
      return nullptr;
    }

    return _connected_peer;
  }

  ENetSocketEvent poll() {
    ENetEvent event;
    int ret = enet_host_service(_host, &event, 0);
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT: {
        std::cout << "INVALID CONNECT EVENT!" << std::endl;
        assert(false); // How can we recieve another connection?! Somebody must be messing arround with the network code.
        break;
      }
      case ENET_EVENT_TYPE_RECEIVE: {
        assert(event.peer == _connected_peer->get_peer());
        const enet_uint8* data = event.packet->data;
        std::string bytes(data, data + event.packet->dataLength - 1); // -1 to crop '\0' ending
        log_recv_packet(bytes, event.channelID);
        _connected_peer->on_message_recieved(bytes, event.channelID);
        enet_packet_destroy(event.packet);
        return ENetSocketEvent(_connected_peer, ENetSocketEvent::MessageRecieved);
      }

      case ENET_EVENT_TYPE_DISCONNECT: {
        std::cout << "RECV: Connection closed!" << std::endl;
        assert(event.peer == _connected_peer->get_peer());
        _connected_peer = nullptr;
        return ENetSocketEvent(nullptr, ENetSocketEvent::Disconnect);
      }
    }

    return ENetSocketEvent(nullptr, ENetSocketEvent::Empty);
  }

private:
  ENetHost* _host = nullptr;

  ENetConnectedPeerPy* _connected_peer = nullptr;
};

ENetClientSocketPy::ENetClientSocketPy() {
  _impl = new impl;
}

ENetClientSocketPy::~ENetClientSocketPy() {
  delete _impl;
}

ENetConnectedPeerPy*
ENetClientSocketPy::connect(const std::string& address, size_t port, size_t max_channels,
                            size_t max_outgoing_bandwith, size_t max_incoming_bandwith,
                            size_t timeout) {
  return _impl->connect(address, port, max_channels, max_outgoing_bandwith, max_incoming_bandwith,
                        timeout);
}

ENetSocketEvent
ENetClientSocketPy::poll() {
  return _impl->poll();
}