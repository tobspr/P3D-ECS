
import random

from message import *
from game_state import *
from entity_mgr import EntityManager
from timesync import TimeSynchronization

from libenet import add_to_packet_log, set_simulation_time

class ClientLogic(object):

    def __init__(self, client_app):
        self.client_app = client_app
        self.deltas = []
        self.waiting_deltas = []
        self.entity_mgr = EntityManager()

        self.player_entity_uuid = None
        self.simulation_time_delta = None

        self.vclient = None # To be set from client
        self.time_sync = TimeSynchronization(self)
        self.initial_synced = False

        self.unconfirmed_events = []

    @property
    def player_entity(self):
        return self.entity_mgr.find(self.player_entity_uuid)

    @property
    def simulation_time(self):
        return self.time_sync.simulation_time

    def handle_message(self, message_id, data):

        if message_id == Message.MID_SYNC_TIME_RESP:
            self.time_sync.handle_time_sync_response(self.vclient, data)

        elif message_id == Message.MID_INIT_GAMESTATE:
            version_no = data["version_no"]
            bulk = data["entities"]

            self.player_entity_uuid = data["player_entity"]
            self.log("Loading", len(bulk), "entities from bulk")
            for entity_data in bulk:
                entity = self.entity_mgr.new_entity()
                entity.load(entity_data)

            # Initial delta
            self.deltas.append(GameDelta(version_no))
            self.vclient.send(Message.MID_CLIENT_READY, {})

        elif message_id == Message.MID_GAMEDELTA:
            self.log("handle incoming gamedelta")
            # Send confirmation
            delta = GameDelta.from_serialized(data)
            self.vclient.send(Message.MID_CONFIRM_DELTA, {"version_no": delta.version_no}, reliable=False)
            if not self.deltas:
                self.log("No connect accept recieved yet, pushing delta to list of waiting deltas")
                self.waiting_deltas.append(delta)
            else:
                version_no = delta.version_no
                last_version_no = self.deltas[-1].version_no
                if version_no <= last_version_no:
                    self.log("Skipping double delta!")
                elif version_no != last_version_no + 1:
                    self.log("Skipping delta which is too far away, got delta with version", version_no, "but can only handle versions up to", last_version_no + 1)
                    self.waiting_deltas.append(delta)
                else:
                    self._handle_delta(delta)

    def _handle_delta(self, delta):
        self.log("Applying delta", delta.version_no, "to entities")
        self.log("Data=", delta)
        for entity_data in delta.changed_entities:
            uuid = entity_data["uuid"]
            entity = self.entity_mgr.find(uuid)
            if not entity:
                # self.error("!GAME STATE CORRUPTED! Entity with uuid", uuid, "was not found!")
                entity = self.entity_mgr.new_entity()
            entity.load(entity_data)

        for event in delta.confirmed_events:
            
            for cmp_event in self.unconfirmed_events:
                if cmp_event.uuid == event.uuid:
                    self.log("Event", cmp_event.uuid, "confirmed by server!")
                    self.unconfirmed_events.remove(cmp_event)
                    cmp_event.apply_corrections(event, self.vclient, self.entity_mgr)
                    break        
            else:
                self.log("New incoming event, which we didn't know about yet: Data:", event)

                event.execute(self.vclient, self.entity_mgr)

        for event_uuid in delta.declined_events:
            
            for cmp_event in self.unconfirmed_events:
                if cmp_event.uuid == event_uuid:
                    self.error("Event", cmp_event, "DECLINED by server")
                    self.error("TODO: RESET entity")
                    self.unconfirmed_events.remove(cmp_event)
                    break
            else:
                self.log("Server declined event we never knew about")

        # processed_events = []
        # for event_uuid in delta.confirmed_events:
        #     for entry in self.unconfirmed_events:
        #         if entry.uuid == event_uuid:
        #             self.log("Server confirmed event", entry.uuid)
        #             processed_events.append(entry)
        # for event in processed_events:
        #     self.unconfirmed_events.remove(event)

        # processed_events = []
        # entities_to_reset = set()
        # for event_uuid in delta.declined_events:
        #     for entry in self.unconfirmed_events:
        #         if entry.uuid == event_uuid:
        #             self.error("Server REJECTED event", entry.uuid)
        #             processed_events.append(entry)
        
        # for event in processed_events:
        #     self.unconfirmed_events.remove(event)

        # for event in self.unconfirmed_events:
        #     # self.log("Pre-Applying unconfirmed event", event)
        #     event.execute(self.vclient, self.entity_mgr)

        self.deltas.append(delta)

    def tick(self, dt):
        set_simulation_time(self.simulation_time)
        self.time_sync.update(self.vclient)

        if not self.initial_synced and self.time_sync.time_information_sufficient:
            # Time synchronized, can connect
            self.vclient.send(Message.MID_CLIENT_INITIAL_SYNC, {})
            self.initial_synced = True

        # Apply waiting deltas
        if self.deltas and self.waiting_deltas:
            did_something = True
            last_version_no = self.deltas[-1].version_no
            while did_something:
                did_something = False
                for delta in self.waiting_deltas:
                    if delta.version_no == last_version_no + 1:
                        self._handle_delta(delta)
                        did_something = True

        self.entity_mgr.update(dt)

    def send_client_delta(self, client_delta):
        if client_delta:
            print("sending client delta:", client_delta)
            self.vclient.send(Message.MID_CLIENT_DELTA, {"delta": client_delta, "timestamp": self.simulation_time})

    def request_event(self, event):
        event.uuid = str(random.randint(10**9, 10**10 - 1))
        event.timestamp = self.time_sync.simulation_time_at_point(self.client_app.last_update)
        self.log("Requesting execution of event", event)
        event.execute(self.vclient, self.entity_mgr)
        self.vclient.send(Message.MID_REQUEST_EVENT, event.serialize())
        self.unconfirmed_events.append(event)


    def log(self, *args):
        # print(*args)
        add_to_packet_log(" ".join([str(i) for i in args]))

    def error(self, *args):
        print("ERROR", *args)
        self.log("ERROR", *args)