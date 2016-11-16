

#include "enet_connected_peer.h"

#include "enet/enet.h"
#include "network_mutex.h"
#include "packet_logging.h"
#include <queue>

struct ENetConnectedPeerPy::impl {

  impl(const std::string& context) : _context(context) {}

  ~impl() {
    if (_peer) {
      enet_peer_reset(_peer);
    }
  }

  bool has_messages() {
    NETWORK_THREAD_SAFE;
    return !_messages.empty();
  }

  std::string take_message() {
    NETWORK_THREAD_SAFE;
    std::string front = _messages.front();
    _messages.pop();
    return std::move(front);
  }

  void set_peer(ENetPeer* peer) { _peer = peer; }
  ENetPeer* get_peer() { return _peer;  }

  void send_message(const std::string& bytes, enet_uint8 channel, bool reliable) {
    NETWORK_THREAD_SAFE;
    log_sent_packet(_context, bytes, channel, reliable);
    assert(_peer);
    ENetPacket* packet =
      enet_packet_create(bytes.c_str(), bytes.size() + 1, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_peer_send(_peer, channel, packet);
  }

  void on_message_recieved(const std::string& bytes, enet_uint8 channel) {
    NETWORK_THREAD_SAFE;
    // TODO: use channel. somehow.
    _messages.push(bytes);
  }

private:
  std::queue<std::string> _messages;
  std::string _context;
  ENetPeer* _peer = nullptr;
};

ENetConnectedPeerPy::ENetConnectedPeerPy(const std::string& context) {
  _impl = new impl(context);
}

ENetConnectedPeerPy::~ENetConnectedPeerPy() {
  delete _impl;
}

bool
ENetConnectedPeerPy::get_has_messages() const {
  return _impl->has_messages();
}

std::string
ENetConnectedPeerPy::take_message() {
  return _impl->take_message();
}

void
ENetConnectedPeerPy::send_message(const std::string& bytes, enet_uint8 channel, bool reliable) {
  _impl->send_message(bytes, channel, reliable);
}


void ENetConnectedPeerPy::set_peer(ENetPeer* peer) {
  _impl->set_peer(peer);
}

ENetPeer* ENetConnectedPeerPy::get_peer() {
  return _impl->get_peer();
}

void ENetConnectedPeerPy::on_message_recieved(const std::string& bytes, enet_uint8 channel) {
  _impl->on_message_recieved(bytes, channel);
}

