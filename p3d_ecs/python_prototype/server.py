
import time
import os

from entity_mgr import EntityManager
from events import *
from components import *
from systems import *
from server_logic import ServerLogic
from virtual_client import VirtualClient

from libenet import *
from direct.showbase.ShowBase import ShowBase


class ServerApp(ShowBase):


    def __init__(self, ip, port):
        ShowBase.__init__(self)
        self.addr = (ip, port)
        self.peer_to_client = {}

        self.addTask(self.update_task, "update")
        self.game_logic = ServerLogic(self)

        # self.renderer = self.game_logic.manager.new_system(RenderSystem)
        self.setBackgroundColor(0.95, 0.95, 0.95, 1.0)
        self.init_socket()

    def init_socket(self):
        self.socket = ENetServerSocketPy()
        if not self.socket.start(self.addr[1]):
            print("FAILED to start server")
            os._exit(-1)

    def update_task(self, task):
        event = self.socket.poll()
        while event.type != ENetSocketEvent.Empty:
            print("Server recieved event: ", event.type)
            event = self.socket.poll()

        processed_clients = []
        for i in range(self.socket.num_peers):
            peer = self.socket.get_peer(i)
            if peer not in self.peer_to_client:
                client = VirtualClient(peer, self.addr)
                self.peer_to_client[peer] = client
                print("NEW CLIENT CONNECTED")
                self.game_logic.handle_new_client(client)
            else:
                client = self.peer_to_client[peer]

            for message_id, data in client.unprocessed_messages:
                self.game_logic.handle_message(client, message_id, data)

            processed_clients.append(peer)

        for peer in list(self.peer_to_client.keys()):
            if peer not in processed_clients:
                print("REMOVING disconnected client", peer)
                del self.peer_to_client[peer]

        self.game_logic.tick(globalClock.getDt())
        return task.cont
