
import time

from panda3d.core import *
from components import *

from libenet import fast_time


class GameEvent(object):

    def __init__(self, data):
        self.data = data
        self.uuid = None
        self.timestamp = None

    def serialize(self):
        return {"uuid": self.uuid, "data": self.data, "timestamp": self.timestamp, "cls": self.__class__.__name__}

    @staticmethod
    def from_serialized(data):
        cls_name = data["cls"]
        instance = globals()[cls_name](data["data"])
        instance.uuid = data["uuid"]
        instance.timestamp = data["timestamp"]
        return instance

    def __repr__(self):
        return str(self.__class__.__name__) + "[uuid=" + str(self.uuid) + ", data=" + str(self.data) + ", timestamp=" + str(self.timestamp) + "]"


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
        entity.get_component(VelocityComponent).velocity = Vec2(*self.data["velocity"]) * speed

    def can_execute(self, vclient, mgr):
        return True
