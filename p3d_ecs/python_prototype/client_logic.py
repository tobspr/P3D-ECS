
import random

from message import *
from game_state import *
from entity_mgr import EntityManager
from timesync import TimeSynchronization
from scheduler import ScheduledQueue

from libenet import add_to_packet_log, set_simulation_time

import config

class ClientLogic(object):

    def __init__(self, client_app):
        self.client_app = client_app
        self.entity_mgr = EntityManager()
        self.time_sync = TimeSynchronization(self)

        self.unconfirmed_events = []

        self.vclient = None # To be set from client
        self.initial_synced = False
        self.deltas = ScheduledQueue()
        self.ready = False

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

            # Get a rough estimate for the simulation index, will be corrected
            # later on
            self.entity_mgr.simulation_index = data["index"]
            
            for entity_data in bulk:
                entity = self.entity_mgr.new_entity()
                entity.load(entity_data)
            self.vclient.send(Message.MID_CLIENT_READY, {}, reliable=True)
            self.ready = True

        elif message_id == Message.MID_SYNC_SIMULATION_INDEX:

            timediff = self.simulation_time - data["timestamp"]
            deltas_to_add = int(timediff / self.entity_mgr.simulation_dt)
            self.entity_mgr.simulation_index = data["index"] + deltas_to_add
            self.log("Recieved sim-index, orig=", data["index"], "corrected=", self.entity_mgr.simulation_index)
            

        elif message_id == Message.MID_GAMEDELTA:
            # todo
            delta = GameDelta.from_serialized(data)

            # Send confirmation
            self.vclient.send(Message.MID_CONFIRM_DELTA, {"version_no": delta.version_no}, reliable=False)

            for other_delta in self.deltas.items:
                if other_delta.version_no == delta.version_no:
                    self.log("Skipping double delta")
                    return

            # self.log("At steps =", self.entity_mgr.simulation_index, "recv. delta for", delta.index)
            self.deltas.append(delta.index, delta)

            for entity_data in delta.changed_entities:
                uuid = entity_data["uuid"]
                if uuid != self.vclient.entity_uuid:
                    entity = self.entity_mgr.find(uuid)
                    if not entity:
                        self.error("Failed to resolve delta, entity with uuid", uuid, "not found")
                        continue
                    entity.load_delta(entity_data["changes"], delta.index * self.entity_mgr.simulation_dt, delta.index)


        elif message_id == Message.MID_REJECT_EVENT:
            self.error("Server rejected event", data["event"])

            affected_event = None
            for event in self.unconfirmed_events:
                if event.uuid == data["event"]:
                    self.unconfirmed_events.remove(event)
                    affected_event = event
                    break

            if not affected_event:
                self.error("Recieved rejection for event which we already recieved or never sent!")
                return

            for entity_data in data["affected_entities"]:
                uuid = entity_data["uuid"]
                entity = self.entity_mgr.find(uuid)
                if not entity:
                    self.error("While correcting rejected event, entity was not found:", uuid)
                    continue
                entity.load(entity_data)


        elif message_id == Message.MID_CONFIRM_EVENT:
            self.log("Server confirmed event", data["event"])

            affected_event = None
            for event in self.unconfirmed_events:
                if event.uuid == data["event"]:
                    self.unconfirmed_events.remove(event)
                    affected_event = event
                    break

            if not affected_event:
                self.error("Recieved confirmation for event which we already recieved or never sent!")
                return

            for entity_data in data["affected_entities"]:
                uuid = entity_data["uuid"]
                entity = self.entity_mgr.find(uuid)
                if not entity:
                    self.error("While correcting accepted event, entity was not found:", uuid)
                    continue
                entity.correct_delta(entity_data["changes"], event.index)

                
    def tick(self, dt):
        set_simulation_time(self.simulation_time)
        self.time_sync.update(self.vclient)


        if not self.initial_synced and self.time_sync.time_information_sufficient:
            # Time synchronized, can connect
            self.vclient.send(Message.MID_CLIENT_INITIAL_SYNC, {}, reliable=True)
            self.initial_synced = True
            return

        if not self.ready:
            return

        # Save deltas
        self.player_entity.save_delta(self.entity_mgr.simulation_index)
        self.player_entity.correct(dt)

        # Apply deltas
        for delta in self.deltas.take_entries(self.entity_mgr.simulation_index):
            if delta.new_entities or delta.changed_entities:
                # self.log("Applying delta", delta.version_no, "-", len(delta.new_entities), "new,", len(delta.changed_entities), "changed")
                pass

            for entity_data in delta.new_entities:
                entity = self.entity_mgr.new_entity()
                entity.load(entity_data)

        for entity in self.entity_mgr.entities:
            if entity.uuid != self.vclient.entity_uuid:
                entity.interpolate(self.simulation_time, self.entity_mgr.simulation_index)

        self.entity_mgr.update(dt)


    def request_event(self, event):
        # if not event.can_execute(self.vclient, self.entity_mgr):
        #     self.log("Can not execute event! can_execute() returned False on client side?!")
        #     return
        
        event.execute(self.vclient, self.entity_mgr)
        event.index = self.entity_mgr.simulation_index
        event.uuid = str(random.randint(10**9, 10**10 - 1))
        # self.log("Sending event", event)
        self.vclient.send(Message.MID_REQUEST_EVENT, event.serialize(), reliable=True)
        self.unconfirmed_events.append(event)
