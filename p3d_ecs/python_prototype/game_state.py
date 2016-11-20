
import copy
import time

from events import *


class GameState(object):

    def __init__(self, version_no, entity_mgr):
        self.version_no = version_no
        self.entities = [entity.serialize() for i in entity_mgr.entities]
        self.timestap = fast_time()

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
            "confirmed_events": list([i.serialize() for i in self.confirmed_events]),
            "declined_events": list(self.declined_events),
            "timestamp": fast_time()
        }

    @property
    def empty(self):
        return not self.changed_entities and not self.confirmed_events and not self.declined_events

    @classmethod
    def from_serialized(cls, data):
        obj = cls(data["version_no"])
        obj.confirmed_events = set([GameEvent.from_serialized(i) for i in data["confirmed_events"]])
        obj.declined_events = set(data["declined_events"])
        obj.changed_entities = data["changed_entities"]
        obj.timestamp = data["timestamp"]
        return obj

    def __repr__(self):
        return "GameDelta[version=" + str(self.version_no) + ", time=" + str(self.timestamp) + \
                ", changed_entities=" + str(self.changed_entities) + ", confirmed_events=" + \
                str(self.confirmed_events) + ", declined_events=" + str(self.declined_events) + "]"
