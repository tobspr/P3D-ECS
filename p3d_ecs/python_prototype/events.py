
from panda3d.core import *
from components import *

class Event(object):

    def __init__(self, data):
        self.data = data
        self.uuid = None

    def serialize(self):
        return {"uuid": self.uuid, "data": self.data}

class EventInteractKeyPressed(Event):
    pass

class EventRequestInteraction(Event):
    pass

class EventMovement(Event):

    @classmethod
    def create(cls, entity, velocity):
        return cls({"entity": str(entity.uuid), "velocity": (velocity.x, velocity.y)})

    def execute(self, mgr):
        entity = mgr.find(self.data["entity"])
        if not entity:
            print("WARNING: Failed to resolve delta, entity", self.data["entity"], "not found")
            return
        entity.get_component(VelocityComponent).velocity = Vec2(*self.data["velocity"])

class EventInteractionRequested(Event):

    @classmethod
    def create(cls, entity):
        return cls({"entity": str(entity.uuid)})

    def execute(self, mgr):
        entity = mgr.find(self.data["entity"])
        if not entity:
            print("WARNING: Failed to resolve delta, entity", self.data["entity"], "not found")
            return
        if entity.has_component(TransformComponent):
            entity.get_component(TransformComponent).rotation += 45
