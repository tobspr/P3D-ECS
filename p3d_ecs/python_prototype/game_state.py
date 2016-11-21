
import copy
import time

from events import *

class GameDelta(object):

    def __init__(self, version_no):
        self.version_no = version_no
        self.index = None
        self.changed_entities = set()
        self.new_entities = set()
        self._serialized = None
        self.sent_timestamp = 0

    def store_serialization(self):
        self._serialized = self.do_serialize()

    def do_serialize(self):
        return {
            "version_no": self.version_no,
            "changed_entities": [i.serialize_changes() for i in self.changed_entities],
            "new_entities": [i.serialize() for i in self.new_entities],
            "index": self.index,
        }
    
    @property
    def serialized(self):
        return self._serialized

    def on_entity_created(self, entity):
        print("Entity created:", entity)
        self.new_entities.add(entity)
        print(self.new_entities)

    @property
    def empty(self):
        return not self.changed_entities and not self.confirmed_events and not self.declined_events

    @classmethod
    def from_serialized(cls, data):
        obj = cls(data["version_no"])
        obj.changed_entities = data["changed_entities"]
        obj.new_entities = data["new_entities"]
        obj.index = data["index"]
        return obj

    def __repr__(self):
        return "GameDelta[version=" + str(self.version_no) + ", index=" + str(self.index) + \
                ", changed_entities=" + str(self.changed_entities) + "]"
