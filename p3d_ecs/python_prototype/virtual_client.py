
import time
import random
from message import Message

from libenet import *


class VirtualClient(object):
    ID_COUNT = 1000

    def __init__(self, connected_peer, addr):
        print("VLIENT::New (", addr, ")")
        self.client_id = self.ID_COUNT
        VirtualClient.ID_COUNT += 1

        self.connected_peer = connected_peer
        self.addr = addr

    def send(self, message_id, data, reliable=True):
        data = Message.make(message_id, data)
        channel = Message.CHANNEL_RELIABLE if reliable else Message.CHANNEL_UNRELIABLE
        self.connected_peer.send_message(data, channel, reliable=reliable)

    @property
    def unprocessed_messages(self):
        messages = []
        while self.connected_peer.has_messages:
            message = self.connected_peer.take_message()
            messages.append(Message.parse(message))
        return messages

    @property
    def prefix(self):
        return "CLIENT:: VirtualClient:: " if self.addr[0] == "client" else "SERVER:: VirtualClient:: "

    def __repr__(self):
        return "CLIENT[" + self.addr[0] + ":" + str(self.addr[1]) + "]"
