
import time
import random
from message import Message

import zlib
from libenet import *


class VirtualClient(object):
    ID_COUNT = 1000

    def __init__(self, connected_peer, addr):
        print("New virtual client (", addr, ")")
        self.client_id = self.ID_COUNT
        VirtualClient.ID_COUNT += 1

        self.connected_peer = connected_peer
        self.addr = addr
        self.ready = False
        self.entity_uuid = None

        self.last_sent_delta = -1
        self.unconfirmed_deltas = set()
        self.last_resend_attempt = 0
        

    def send(self, message_id, data, reliable, channel=None):
        data = Message.make(message_id, data)
        send_channel = Message.CHANNEL_RELIABLE if reliable else Message.CHANNEL_UNRELIABLE
        if channel:
            send_channel = channel
        self.connected_peer.send_message(data, send_channel, reliable=reliable)

    @property
    def unprocessed_messages(self):
        messages = []
        while self.connected_peer.has_messages:
            message = self.connected_peer.take_message()
            messages.append(Message.parse(message))
        return messages

    def __repr__(self):
        return "CLIENT[" + self.addr[0] + ":" + str(self.addr[1]) + "]"
