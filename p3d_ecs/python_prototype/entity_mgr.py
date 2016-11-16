

import random

from panda3d.core import *
from components import *

class Entity(object):

    def __init__(self, mgr):
        self._components = {}
        self._manager = mgr
        self.uuid = str(random.randint(10**9, 10**10 - 1))

    def has_component(self, comp_cls):
        return comp_cls in self._components

    def get_component(self, comp_cls):
        assert self.has_component(comp_cls)
        return self._components[comp_cls]

    def new_component(self, comp_cls):
        assert not self.has_component(comp_cls)
        instance = comp_cls(self)
        self._components[comp_cls] = instance
        return instance

    def serialize(self):
        return {
            "uuid": self.uuid,
            "components": {
                str(k.__name__): v.serialize() for k, v in self._components.items()
            }
        }

    def load(self, json):
        self.uuid = json["uuid"]
        components_ok = []
        for cls_name, data in json["components"].items():
            type_handle = globals()[cls_name]
            if not self.has_component(type_handle):
                print("Adding new component", type_handle)
                self.new_component(type_handle)
            self.get_component(type_handle).load(data)
            components_ok.append(type_handle)
        bad_comps = []
        for comp in self._components.keys():
            if comp not in components_ok:
                bad_comps.append(comp)
        for comp in bad_comps:
            print("Removing outdated component", comp)
            self._components[comp].clear()
            del self._components[comp]

    def clear(self):
        for component in self._components.items():
            component.clear()
            del component
        self._components = {}

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
        self.entites = []

    def get_gamestate(self):
        return [entity.serialize() for entity in self.entities]
