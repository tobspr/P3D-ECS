
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

    UPDATE_RATE = 5

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
        self.addTask(self.watch_entities, "watch_entities", sort=10)

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
        self.accept("w", self.set_velocity, [None, 1])
        self.accept("a", self.set_velocity, [-1, None])
        self.accept("s", self.set_velocity, [None, -1])
        self.accept("d", self.set_velocity, [1, None])

        self.accept("w-up", self.clear_velocity, [None, 1])
        self.accept("a-up", self.clear_velocity, [-1, None])
        self.accept("s-up", self.clear_velocity, [None, -1])
        self.accept("d-up", self.clear_velocity, [1, None])

        self.accept("e", self.try_interact)

    def try_interact(self):
        # interacts = self.interact_system._available_interacts
        # if interacts:
        #     print("Trying to interact with", interacts[0])
        #     event = EventInteractionRequested.create(interacts[0])
        #     self.game_logic.trigger_event(self.game_logic.vclient, event)

        # self.manager.trigger_event(EventInteractKeyPressed())
        pass

    def clear_velocity(self, x, y):
        if self.game_logic.player_entity:
            c_velocity = self.game_logic.player_entity.get_component(VelocityComponent)

            def sign(x):
                return math.copysign(1, x)

            if x is not None and sign(x) == sign(c_velocity.velocity.x):
                c_velocity.velocity.x = 0

            if y is not None and sign(y) == sign(c_velocity.velocity.y):
                c_velocity.velocity.y = 0

    def set_velocity(self, x, y):
        speed = 2.0
        if self.game_logic.player_entity:
            c_velocity = self.game_logic.player_entity.get_component(VelocityComponent)

            if x is not None:
                c_velocity.velocity.x = x * speed

            if y is not None:
                c_velocity.velocity.y = y * speed

    def watch_entities(self, task):
        if not self.game_logic.player_entity:
            return task.cont

        if fast_time() - self.last_watch < 1.0 / self.UPDATE_RATE:
            return task.cont

        self.last_watch = fast_time()

        client_delta = self.game_logic.player_entity.get_changed_vars()
        self.game_logic.send_client_delta(client_delta)

        return task.cont
