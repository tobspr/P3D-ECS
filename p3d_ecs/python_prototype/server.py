
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

GUI_INTERFACE = True

class FakeTask(object):
    cont = None

class ServerApp(ShowBase if GUI_INTERFACE else object):

    UPDATE_RATE = 60

    def __init__(self, ip, port):
        set_context("SERVER")
        if GUI_INTERFACE:
            ShowBase.__init__(self)
            self.addTask(self.update_task, "update")

        self.addr = (ip, port)
        self.peer_to_client = {}
        self.last_update = fast_time()

        self.game_logic = ServerLogic(self)
        self.game_logic.load()

        if GUI_INTERFACE:
            self.renderer = self.game_logic.entity_mgr.new_system(RenderSystem)
            self.setBackgroundColor(0.95, 0.95, 0.95, 1.0)

        self.physics = self.game_logic.entity_mgr.new_system(PhysicsSystem)

        self.init_socket()
        if not GUI_INTERFACE:
            while True:
                self.update_task(FakeTask)

    def init_socket(self):
        self.socket = ENetServerSocketPy()
        if not self.socket.start(self.addr[1], max_channels=3):
            print("FAILED to start server")
            os._exit(-1)

    def update_task(self, task):

        duration = fast_time() - self.last_update
        if duration < 1.0 / self.UPDATE_RATE:
            return task.cont
        self.last_update = fast_time()

        event = self.socket.poll()
        while event.type != ENetSocketEvent.Empty:
            event = self.socket.poll()

        processed_clients = []
        for i in range(self.socket.num_peers):
            peer = self.socket.get_peer(i)
            if peer not in self.peer_to_client:
                client = VirtualClient(peer, self.addr)
                self.peer_to_client[peer] = client
                self.game_logic.handle_new_client(client)
            else:
                client = self.peer_to_client[peer]

            for message_id, data in client.unprocessed_messages:
                self.game_logic.handle_message(client, message_id, data)

            processed_clients.append(peer)

        for peer in list(self.peer_to_client.keys()):
            if peer not in processed_clients:
                print("removing disconnected client", peer)
                del self.peer_to_client[peer]

        self.game_logic.tick(duration)

        for entity in self.game_logic.entity_mgr.entities:
            entity.interpolate(self.game_logic.simulation_time - 300.0 / 1000.0)

        return task.cont
