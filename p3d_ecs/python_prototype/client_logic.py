
import random

from message import *
from game_state import *
from entity_mgr import EntityManager
from timesync import TimeSynchronization
from scheduler import ScheduledQueue

from libenet import add_to_packet_log, set_simulation_time

class ClientLogic(object):

    def __init__(self, client_app):
        self.client_app = client_app
        self.entity_mgr = EntityManager()
        self.time_sync = TimeSynchronization(self)

        self.vclient = None # To be set from client
        self.initial_synced = False
        self.deltas = ScheduledQueue()

    def log(self, *args):
        print(*args)
        add_to_packet_log(" ".join([str(i) for i in args]))

    def error(self, *args):
        print("ERROR", *args)
        self.log("ERROR", *args)

    @property
    def player_entity(self):
        return self.entity_mgr.find(self.vclient.entity_uuid)

    @property
    def simulation_time(self):
        return self.time_sync.simulation_time

    def handle_message(self, message_id, data):

        if message_id == Message.MID_SYNC_TIME_RESP:
            self.time_sync.handle_time_sync_response(self.vclient, data)

        elif message_id == Message.MID_INIT_GAMESTATE:
            bulk = data["entities"]

            self.vclient.entity_uuid = data["player_entity"]
            self.log("Loading", len(bulk), "entities from bulk")
            for entity_data in bulk:
                entity = self.entity_mgr.new_entity()
                entity.load(entity_data)
            self.vclient.send(Message.MID_CLIENT_READY, {})

        elif message_id == Message.MID_GAMEDELTA:
            # todo
            delta = GameDelta.from_serialized(data)

            # self.log("At time =", self.simulation_time, "recv. delta for", delta.timestamp)
            self.deltas.append(delta.timestamp, delta)

            for entity_data in delta.changed_entities:
                uuid = entity_data["uuid"]
                if uuid != self.vclient.entity_uuid:
                    entity = self.entity_mgr.find(uuid)
                    if not entity:
                        self.error("Failed to resolve delta, entity with uuid", uuid, "not found")
                        continue
                    entity.load_delta(entity_data["changes"], delta.timestamp)

    def tick(self, dt):
        set_simulation_time(self.simulation_time)
        self.time_sync.update(self.vclient)

        if not self.initial_synced and self.time_sync.time_information_sufficient:
            # Time synchronized, can connect
            self.vclient.send(Message.MID_CLIENT_INITIAL_SYNC, {})
            self.initial_synced = True

        # Apply deltas
        for delta in self.deltas.take_entries(self.simulation_time):
            if delta.new_entities or delta.changed_entities:
                self.log("Applying delta", delta.version_no, "-", len(delta.new_entities), "new,", len(delta.changed_entities), "changed")

            for entity_data in delta.new_entities:
                entity = self.entity_mgr.new_entity()
                entity.load(entity_data)

        for entity in self.entity_mgr.entities:
            if entity.uuid != self.vclient.entity_uuid:
                entity.interpolate(self.simulation_time)

        self.entity_mgr.update(dt)


    def request_event(self, event):
        if not event.can_execute(self.vclient, self.entity_mgr):
            self.log("Can not execute event! can_execute() returned False on client side?!")
            return
        
        event.execute(self.vclient, self.entity_mgr)
        event.timestamp = self.simulation_time
        self.vclient.send(Message.MID_REQUEST_EVENT, event.serialize())
