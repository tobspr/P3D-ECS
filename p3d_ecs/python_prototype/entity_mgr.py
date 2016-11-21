

import random

from panda3d.core import *

from components import *
from entity import Entity

import config

class EntityManager(object):

    def __init__(self):
        self.entities = []
        self.systems = []
        self.creation_callback = None
        self.accum_time = 0.0
        self.simulation_index = 0

    @property
    def simulation_dt(self):
        return 1.0 / config.UPDATE_RATE

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

        if dt > self.simulation_dt:
            # print("CAN'T KEEP UP! Load too heavy? We may only use {:3.3f} ms but used {:3.3f} ms".format(self.simulation_dt * 1000.0, dt * 1000.0))
            pass

        self.accum_time += dt

        # Process fixed step systems first
        while self.accum_time >= self.simulation_dt:
            self.accum_time -= self.simulation_dt
            self.simulation_index += 1
            for system in self.systems:
                if system.FIXED_STEP:
                    matching_entities = []
                    for entity in self.entities:
                        for required in system.REQUIRES:
                            if not entity.has_component(required):
                                break
                        else:
                            matching_entities.append(entity)
                    system.process(matching_entities, self.simulation_dt)

        # Now process all other systems 
        for system in self.systems:
            if not system.FIXED_STEP:
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
