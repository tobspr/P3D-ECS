
from message import Message

class ServerLogic(object):

    def __init__(self, server_app):
        self.server_app = server_app

    def load(self):
        pass

    def handle_new_client(self, client):
        pass

    def handle_message(self, client, message_id, data):
        if message_id == Message.MID_CONNECT:
            client.send(Message.MID_CONNECT_ACCEPT, {})

    def tick(self, dt):
        pass