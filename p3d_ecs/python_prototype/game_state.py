
import copy
import time

from events import *

class GameState(object):

    def __init__(self, version_no, entities):
        self.version_no = version_no
        self.entities = [entity.serialize() for i in entities]
        self.timestap = time.time()

    def serialize(self):
        return self.entities


class BaseGameAction(object):
    def __init__(self, data):
        self.data = data
        self.data["_timestamp"] = self.data.get("_timestamp", time.time())

    def serialize(self):
        return self.data

class GameActionCreateEntity(BaseGameAction):

    @classmethod
    def from_entity(cls, entity):
        return cls({"uuid": str(entity.uuid)})

    def execute(self, mgr):
        entity = mgr.new_entity()
        entity.uuid = str(self.data["uuid"])

class GameActionModifyEntity(BaseGameAction):

    @classmethod
    def from_entity(cls, entity):
        return cls({"uuid": str(entity.uuid), "raw_data": copy.deepcopy(entity.serialize())})

    def execute(self, mgr):
        entity = mgr.find(self.data["uuid"])
        if not entity:
            print("WARNING: Failed to resolve delta, creating entity", self.data["uuid"])
            entity = mgr.new_entity()

        entity.load(self.data["raw_data"])

class GameActionExecuteEvent(BaseGameAction):

    @classmethod
    def from_event(cls, event):
        return cls({"event_uuid": event.uuid, "event_class": event.__class__.__name__, "event_data": copy.deepcopy(event.serialize())})

    def execute(self, mgr):
        instance = globals()[self.data["event_class"]](self.data["event_data"])
        instance.execute(mgr)

    @property
    def event_uuid(self):
        return self.data["event_uuid"]