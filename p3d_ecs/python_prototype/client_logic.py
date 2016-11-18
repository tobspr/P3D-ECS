
import random

from message import *
from game_state import *
from entity_mgr import EntityManager


from libenet import add_to_packet_log, set_simulation_time

class ClientLogic(object):

    def __init__(self, client_app):
        self.client_app = client_app
        self.deltas = []
        self.waiting_deltas = []
        self.entity_mgr = EntityManager()

        self.player_entity_uuid = None
        self.simulation_time_delta = None

    @property
    def player_entity(self):
        return self.entity_mgr.find(self.player_entity_uuid)

    @property
    def simulation_time(self):
        if not self.simulation_time_delta:
            return 0.0
        return time.time() + self.simulation_time_delta

    def handle_message(self, message_id, data):

        if message_id == Message.MID_INIT_GAMESTATE:
            version_no = data["version_no"]
            bulk = data["entities"]

            self.player_entity_uuid = data["player_entity"]
            self.log("Loading", len(bulk), "entities from bulk")
            for entity_data in bulk:
                entity = self.entity_mgr.new_entity()
                entity.load(entity_data)

            self.simulation_time_delta = data["simulation_time"] - time.time()
            self.log("Simulation time delta =", self.simulation_time_delta, "-> current time", self.simulation_time)

            # Initial delta
            self.deltas.append(GameDelta(version_no))
            self.client_app.vclient.send(Message.MID_CLIENT_READY, {})

        elif message_id == Message.MID_GAMEDELTA:
            self.log("handle incoming gamedelta")
            # Send confirmation
            delta = GameDelta.from_serialized(data)
            self.client_app.vclient.send(Message.MID_CONFIRM_DELTA, {"version_no": delta.version_no}, reliable=False)
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
                self.log("!GAME STATE CORRUPTED! Entity with uuid", uuid, "was not found!")
                break
            entity.load(entity_data)
        self.deltas.append(delta)

    def tick(self, dt):
        set_simulation_time(self.simulation_time)

        # Apply waiting deltas
        if self.deltas and self.waiting_deltas:
            did_something = True
            last_version_no = self.deltas[-1].version_no
            while did_something:
                did_something = False
                for delta in self.waiting_deltas:
                    if delta["version_no"] == last_version_no + 1:
                        self._handle_delta(delta)
                        did_something = True

        self.entity_mgr.update(dt)

    def request_event(self, event):
        self.log("Requesting execution of event", event)
        event.uuid = str(random.randint(10**9, 10**10 - 1))

        data = event.serialize()
        self.client_app.vclient.send(Message.MID_REQUEST_EVENT, {"timestamp": self.simulation_time, "event": data})


    def log(self, *args):
        # print(*args)
        add_to_packet_log(" ".join([str(i) for i in args]))
