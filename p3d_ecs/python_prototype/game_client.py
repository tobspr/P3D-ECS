
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
        self.addTask(self.update, "update", sort=38)

        self._velocity = 0, 0
        self.movements = {"top": 0, "right": 0, "bottom": 0, "left": 0}

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

        if self.game_logic.player_entity:
            event = EventMovement.create(Vec2(*self._velocity))
            self.game_logic.request_event(event)


        self.game_logic.tick(globalClock.get_dt())

        return task.cont

    def init_input(self):
        for suffix in ("",):
            self.accept("w" + suffix, self.set_velocity, ["top"])
            self.accept("a" + suffix, self.set_velocity, ["left"])
            self.accept("s" + suffix, self.set_velocity, ["bottom"])
            self.accept("d" + suffix, self.set_velocity, ["right"])

        self.accept("w-up", self.clear_velocity, ["top"])
        self.accept("a-up", self.clear_velocity, ["left"])
        self.accept("s-up", self.clear_velocity, ["bottom"])
        self.accept("d-up", self.clear_velocity, ["right"])

    def clear_velocity(self, idx):
        if self.game_logic.player_entity:
            self.movements[idx] = 0
            self.recompute_movement()

    def set_velocity(self, idx):
        speed = 2.0

        # TODO: This is TOTALLY overcomplicated. could solve it much, much simpler.
        if self.game_logic.player_entity:
            if idx in ("left", "right"):
                self.movements["left"] = 0
                self.movements["right"] = 0
            elif idx in ("top", "bottom"):
                self.movements["top"] = 0
                self.movements["bottom"] = 0

            self.movements[idx] = 1
            self.recompute_movement()

    def recompute_movement(self):
        vel_x, vel_y = 0, 0
        if self.movements["left"]:
            vel_x -= 1
        if self.movements["right"]:
            vel_x += 1
        if self.movements["top"]:
            vel_y += 1
        if self.movements["bottom"]:
            vel_y -= 1

        self._velocity = vel_x, vel_y