
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

    def has_changes(self):
        for comp in self._components.values():
            if comp.has_changes():
                return True
        return False

    def serialize_changes(self):
        result = {}
        for k, comp in self._components.items():
            changed = comp.get_changed_vars()
            if changed:
                result[k.__name__] = changed
        return {"uuid": self.uuid, "changes": result}

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
                # print("Adding new component", type_handle)
                self.new_component(type_handle)
            self.get_component(type_handle).load(data)
            components_ok.append(type_handle)
        bad_comps = []
        for comp in self._components.keys():
            if comp not in components_ok:
                bad_comps.append(comp)
        for comp in bad_comps:
            # print("Removing outdated component", comp)
            self._components[comp].clear()
            del self._components[comp]

    def load_delta(self, json, timestamp):
        
        for cls_name, data in json.items():
            type_handle = globals()[cls_name]
            component = self.get_component(type_handle)
            component.load_delta(data, timestamp)

    def interpolate(self, timestamp):
        for component in self._components.values():
            component.interpolate(timestamp)

    def clear(self):
        for component in self._components.items():
            component.clear()
            del component
        self._components = {}
