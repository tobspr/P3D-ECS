import os
import time

from panda3d.core import *
# from direct.gui.DirectGui import DirectFrame
from direct.showbase.ShowBase import ShowBase

from events import *
from components import *
from systems import *
from message import *
from client_logic import ClientLogic

from virtual_client import VirtualClient
from libenet import *

class GameClient(ShowBase):

    def __init__(self, ip, port):
        set_context("CLIENT")
        super().__init__()
        self.addr = ip, port
        self.game_logic = ClientLogic(self)
        self.init_networking()
        self.init_systems()
        self.init_input()

        self.addTask(self.update, "update")

    def init_systems(self):
        self.render_system = self.game_logic.entity_mgr.new_system(RenderSystem)
        # self.interact_system = self.game_logic.manager.new_system(
        #     InteractSystem)


    def init_networking(self):
        self.socket = ENetClientSocketPy()
        self.peer = self.socket.connect(self.addr[0], self.addr[1])
        if not self.peer:
            print("FAILED to connect")
            os._exit(-1)

        self.vclient = VirtualClient(self.peer, ("client", 0))
        self.vclient.send(Message.MID_CONNECT, {})

    def update(self, task):
        event = self.socket.poll()
        while event.type != ENetSocketEvent.Empty:
            # print("Client recieved event: ", event.type)
            event = self.socket.poll()

        for message_id, data in self.vclient.unprocessed_messages:
            self.game_logic.handle_message(message_id, data)

        # self.interact_system.player = self.player_entity
        self.game_logic.tick(globalClock.get_dt())
        return task.cont

    # @property
    # def player_entity(self):
    #     return self.game_logic.manager.find(self.vclient.assigned_entity)

    def init_input(self):
        self.accept("w", self.set_velocity, [0, 1])
        self.accept("a", self.set_velocity, [-1, 0])
        self.accept("s", self.set_velocity, [0, -1])
        self.accept("d", self.set_velocity, [1, 0])
        for k in "wasd":
            self.accept(k + "-up", self.set_velocity, [0, 0])
        self.accept("e", self.try_interact)
        pass

    def try_interact(self):
        # interacts = self.interact_system._available_interacts
        # if interacts:
        #     print("Trying to interact with", interacts[0])
        #     event = EventInteractionRequested.create(interacts[0])
        #     self.game_logic.trigger_event(self.vclient, event)

        # self.manager.trigger_event(EventInteractKeyPressed())
        pass

    def set_velocity(self, x, y):
        speed = 1.0
        if self.game_logic.player_entity:
            # vel = self.player_entity.get_component(VelocityComponent)
            # vel.velocity = Vec2(x, y) * speed
            event = EventMovement.create(self.game_logic.player_entity, Vec2(x, y) * speed)
            self.game_logic.request_event(event)
        pass
