
import os
import time
import math
import _thread

from panda3d.core import *
from direct.showbase.ShowBase import ShowBase

from events import *
from components import *
from systems import *
from client_logic import ClientLogic

from virtual_client import VirtualClient
from libenet import *


class GameClient(ShowBase):

    def __init__(self, ip, port):
        set_context("CLIENT")
        super().__init__()
        self.addr = ip, port
        self.game_logic = ClientLogic(self)
        self.init_systems()
        self.init_input()
        self.init_networking()

        self.last_watch = fast_time()
        self.addTask(self.update, "update", sort=0)

    def init_systems(self):
        self.render_system = self.game_logic.entity_mgr.new_system(RenderSystem)
        self.physics_system = self.game_logic.entity_mgr.new_system(PhysicsSystem)

    def init_networking(self):
        self.socket = ENetClientSocketPy()
        self.peer = self.socket.connect(self.addr[0], self.addr[1], max_channels=3)
        if not self.peer:
            print("FAILED to connect")
            os._exit(-1)

        vclient = VirtualClient(self.peer, ("client", 0))
        self.game_logic.vclient = vclient

    def update(self, task):
        event = self.socket.poll()
        while event.type != ENetSocketEvent.Empty:
            event = self.socket.poll()

        for message_id, data in self.game_logic.vclient.unprocessed_messages:
            self.game_logic.handle_message(message_id, data)

        self.game_logic.tick(globalClock.get_dt())
        return task.cont

    def init_input(self):
        for suffix in ("",):
            self.accept("w" + suffix, self.set_velocity, [None, 1])
            self.accept("a" + suffix, self.set_velocity, [-1, None])
            self.accept("s" + suffix, self.set_velocity, [None, -1])
            self.accept("d" + suffix, self.set_velocity, [1, None])

        self.accept("w-up", self.clear_velocity, [None, 1])
        self.accept("a-up", self.clear_velocity, [-1, None])
        self.accept("s-up", self.clear_velocity, [None, -1])
        self.accept("d-up", self.clear_velocity, [1, None])

    def clear_velocity(self, x, y):
        if self.game_logic.player_entity:
            event = EventMovement.create(Vec2(0, 0))
            self.game_logic.request_event(event)

    def set_velocity(self, x, y):
        speed = 2.0
        if self.game_logic.player_entity:
            event = EventMovement.create(Vec2(x or 0, y or 0))
            self.game_logic.request_event(event)
