

import random

from panda3d.core import *

from components import *
from entity import Entity


class EntityManager(object):

    def __init__(self):
        self.entities = []
        self.systems = []
        self.creation_callback = None

    def new_entity(self):
        entity = Entity(self)
        self.entities.append(entity)
        if self.creation_callback:
            self.creation_callback(entity)
        return entity

    def new_system(self, cls_handle, *args):
        self.systems.append(cls_handle(self, *args))
        return self.systems[-1]

    def update(self, dt=0.01):
        for system in self.systems:
            matching_entities = []
            for entity in self.entities:
                for required in system.REQUIRES:
                    if not entity.has_component(required):
                        break
                else:
                    matching_entities.append(entity)
            system.process(matching_entities, dt)

    def trigger_event(self, event):
        for system in self.systems:
            if event.__class__ in system.CATCH_EVENTS:
                processed = system.process_event(event)
                if processed:
                    return

        print("WARNING: Unhandled event!", event)

    def find(self, uuid):
        for entity in self.entities:
            if entity.uuid == uuid:
                return entity
        return None

    def load_entity(self, json):
        entity = self.new_entity()
        entity.load(json)
        return entity

    def clear(self):
        for entity in self.entities:
            entity.clear()
            del entity
        self.entities = []

    def get_gamestate(self):
        return [entity.serialize() for entity in self.entities]
