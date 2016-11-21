
import time
import random

from panda3d.core import *
from components import *

from libenet import fast_time


class GameEvent(object):

    def __init__(self, data):
        self.data = data
        self.uuid = None
        self.index = None

    def serialize(self):
        return {"uuid": self.uuid, "data": self.data, "index": self.index, "cls": self.__class__.__name__}

    @staticmethod
    def from_serialized(data):
        cls_name = data["cls"]
        instance = globals()[cls_name](data["data"])
        instance.uuid = data["uuid"]
        instance.index = data["index"]
        return instance

    def get_affected_entities(self, vclient, mgr):
        return []

    def __repr__(self):
        return str(self.__class__.__name__) + "[uuid=" + str(self.uuid) + ", data=" + str(self.data) + ", index=" + str(self.index) + "]"


class EventMovement(GameEvent):

    @classmethod
    def create(cls, velocity):
        return cls({"velocity": (velocity.x, velocity.y)})

    def execute(self, vclient, mgr):
        entity = mgr.find(vclient.entity_uuid)
        if not entity:
            print("WARNING: Failed to resolve delta, entity", vclient.entity_uuid, "not found")
            return

        speed = 1.4
        vel = Vec2(*self.data["velocity"])
        vel.normalize()

        entity.get_component(VelocityComponent).velocity = vel * speed

    def can_execute(self, vclient, mgr):
        return True

    def get_affected_entities(self, vclient, mgr):
        return [vclient.entity_uuid]
