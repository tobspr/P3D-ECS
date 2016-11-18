
import copy
import time

from events import *


class GameState(object):

    def __init__(self, version_no, entity_mgr):
        self.version_no = version_no
        self.entities = [entity.serialize() for i in entity_mgr.entities]
        self.timestap = time.time()

    def serialize(self):
        return {"timestamp": self.timestamp, "version_no": self.version_no, "entities": self.entities}


class GameDelta(object):

    def __init__(self, version_no):
        self.version_no = version_no
        self.timestamp = None
        self.changed_entities = set()
        self.confirmed_events = set()
        self.declined_events = set()

    def serialize(self):
        return {
            "version_no": self.version_no,
            "changed_entities": [i.serialize() for i in self.changed_entities],
            "confirmed_events": list(self.confirmed_events),
            "declined_events": list(self.declined_events),
            "timestamp": time.time()
        }

    @property
    def empty(self):
        return not self.changed_entities and not self.confirmed_events and not self.declined_events

    @classmethod
    def from_serialized(cls, data):
        obj = cls(data["version_no"])
        obj.confirmed_events = set(data["confirmed_events"])
        obj.declined_events = set(data["declined_events"])
        obj.changed_entities = data["changed_entities"]
        obj.timestamp = data["timestamp"]
        return obj

# class BaseGameAction(object):
#     def __init__(self, data):
#         self.data = data
#         self.data["_timestamp"] = self.data.get("_timestamp", time.time())

#     def serialize(self):
#         return self.data

# class GameActionCreateEntity(BaseGameAction):

#     @classmethod
#     def from_entity(cls, entity):
#         return cls({"uuid": str(entity.uuid)})

#     def execute(self, mgr):
#         entity = mgr.new_entity()
#         entity.uuid = str(self.data["uuid"])

# class GameActionModifyEntity(BaseGameAction):

#     @classmethod
#     def from_entity(cls, entity):
# return cls({"uuid": str(entity.uuid), "raw_data":
# copy.deepcopy(entity.serialize())})

#     def execute(self, mgr):
#         entity = mgr.find(self.data["uuid"])
#         if not entity:
#             print("WARNING: Failed to resolve delta, creating entity", self.data["uuid"])
#             entity = mgr.new_entity()

#         entity.load(self.data["raw_data"])

# class GameActionExecuteEvent(BaseGameAction):

#     @classmethod
#     def from_event(cls, event):
# return cls({"event_uuid": event.uuid, "event_class":
# event.__class__.__name__, "event_data":
# copy.deepcopy(event.serialize())})

#     def execute(self, mgr):
#         instance = globals()[self.data["event_class"]](self.data["event_data"])
#         instance.execute(mgr)

#     @property
#     def event_uuid(self):
#         return self.data["event_uuid"]
