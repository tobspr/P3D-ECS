
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

    def get_affected_entities(self):
        return []

    def __repr__(self):
        return str(self.__class__.__name__) + "[uuid=" + str(self.uuid) + ", data=" + str(self.data) + ", timestamp=" + str(self.timestamp) + "]"



class EventMovement(GameEvent):

    @classmethod
    def create(cls, entity):
        pos = entity.get_component(TransformComponent).pos
        velocity = entity.get_component(VelocityComponent).velocity
        return cls({"entity": str(entity.uuid), "pos": (pos.x, pos.y), "velocity": (velocity.x, velocity.y)})

    def execute(self, vclient, mgr):
        entity = mgr.find(self.data["entity"])
        if not entity:
            print("WARNING: Failed to resolve delta, entity", self.data["entity"], "not found")
            return

        entity.get_component(VelocityComponent).velocity = Vec2(*self.data["velocity"])
        return [entity]

    def update(self, vclient, mgr):
        entity = mgr.find(self.data["entity"])
        if entity:
            pos = entity.get_component(TransformComponent).pos
            self.data["pos"] = pos.x, pos.y

    def apply_corrections(self, server_event, vclient, mgr):
        entity = mgr.find(self.data["entity"])
        
        if self.data["velocity"] != (0, 0):

            if entity:
                pos = self.data["pos"]
                server_pos = server_event.data["pos"]
                diff_x, diff_y = pos[0] - server_pos[0], pos[1] - server_pos[1]
                print("DELTA:", pos, server_pos)
                entity.get_component(TransformComponent).pos -= Vec2(diff_x, diff_y)

    def get_affected_entities(self):
        return [self.data["entity"]]

    def can_execute(self, vclient, mgr):
        if vclient.entity_uuid != self.data["entity"]:
            print("ERROR: Client attempted to control entity without permission!")
            return False

        # return False

        return True
