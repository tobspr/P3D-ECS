
import time
import random

from message import Message
from entity_mgr import EntityManager
from components import *
from game_state import GameState, GameDelta
from events import GameEvent
from libenet import add_to_packet_log, set_simulation_time, fast_time


class ServerLogic(object):

    # Time to wait in seconds before resending unconfirmed deltas
    DELTA_RESEND_RATE = 0.5

    # Amount of unconfirmed deltas until disconnecting the client
    MAX_UNCONFIRMED_DELTAS = 10

    # Disconnect clients after recieving no packet after X seconds
    MAX_QUIET_TIME = 10.0

    # Time to wait until resending a delta, this avoids sending deltas
    # twice which are then confirmed a few milliseconds later on
    DELTA_RESEND_WAIT = 0.1

    # Usually no deltas are created, unless changes occured. To ensure a stable
    # connection, deltas are also generated at a fixed interval, even when no
    # changes occured. This is the time of the interval
    AUTO_DELTA_RATE = 5.0

    # Process events a bit later to accumulate for varying ping times
    EVENT_PROCESSING_DELAY = 0.2

    def __init__(self, server_app):
        self.server_app = server_app
        self.entity_mgr = EntityManager()
        self.deltas = []
        self.event_queue = []
        self.current_delta = GameDelta(1)
        self.clients = set()
        self.start_time = fast_time()
        self.last_delta_sent = 0

    @property
    def simulation_time(self):
        return fast_time() - self.start_time

    @property
    def event_simulation_time(self):
        return self.simulation_time - self.EVENT_PROCESSING_DELAY

    def load(self):
        self.log("ServerLogic::load")

        # TODO: Load from file
        door = self.entity_mgr.new_entity()
        transform = door.new_component(TransformComponent)
        draw = door.new_component(DrawComponent)
        interact = door.new_component(InteractableComponent)
        interact.radius = 0.3

        transform.pos_x.v = 0.7
        transform.pos_y.v = 0.1
        transform.rotation = 45
        draw.load({"mesh": (0.2, 0.6, 0.3, 1.0)})
        self.current_delta.changed_entities.add(door)

        # Stress test
        # for i in range(100):
        #     entity = self.entity_mgr.new_entity()
        #     transform = entity.new_component(TransformComponent)
        #     transform.pos.x = random.random()
        #     transform.pos.y = random.random()

    def handle_new_client(self, client):
        self.clients.add(client)

    def handle_message(self, client, message_id, data):
        if message_id == Message.MID_SYNC_TIME:

            client.send(Message.MID_SYNC_TIME_RESP, {
                        "id": data["id"],
                        "simulation_time": self.simulation_time
                        }, reliable=False, channel=Message.CHANNEL_TIMESYNC)

        elif message_id == Message.MID_CLIENT_INITIAL_SYNC:

            if client.ready:
                self.log(
                    "Client is already synced, but rerequested sync! Bad client?")
                return

            # Spawn player entity
            player_entity = self.entity_mgr.new_entity()
            transform = player_entity.new_component(TransformComponent)
            draw = player_entity.new_component(DrawComponent)
            vel = player_entity.new_component(VelocityComponent)
            transform.pos_x.v = 0.2
            transform.pos_y.v = 0.2
            draw.load({"mesh": (1.0, 0.6, 0.2, 1.0)})

            self.current_delta.changed_entities.add(player_entity)
            
            # Make sure we have the newest game state
            self.swap_delta()
            client.entity_uuid = player_entity.uuid

            # Send accept connect, and also include all entities,
            # so we can push the clients version_no
            client.send(Message.MID_INIT_GAMESTATE, {
                        "version_no": self.deltas[-1]["version_no"],
                        "entities": [entity.serialize() for entity in self.entity_mgr.entities],
                        "player_entity": player_entity.uuid,
                        "simulation_time": self.simulation_time,
                        })

            client.last_sent_delta = self.deltas[-1]["version_no"]

        elif message_id == Message.MID_CLIENT_READY:
            self.log("Client", client, "is now ready, and at version",
                     client.last_sent_delta)
            client.ready = True
            client.last_confirmation_time = fast_time()

        elif message_id == Message.MID_CONFIRM_DELTA:

            version_no = data["version_no"]
            if version_no in client.unconfirmed_deltas:
                self.log("Client confirmed game delta", version_no)
                client.unconfirmed_deltas.remove(version_no)
                client.last_confirmation_time = fast_time()

        elif message_id == Message.MID_REQUEST_EVENT:

            # todo: validate event time

            event = GameEvent.from_serialized(data)
            self.log("Client requested execution event of type",
                     event.__class__.__name__, "at time", event.timestamp)
            event.client = client  # required for processing
            self.event_queue.append(event)

    def swap_delta(self):
        needs_delta = (
            fast_time() - self.last_delta_sent) > self.AUTO_DELTA_RATE
        if needs_delta or not self.current_delta.empty:
            self.last_delta_sent = fast_time()
            old_delta = self.current_delta
            self.current_delta = GameDelta(old_delta.version_no + 1)
            self.deltas.append(old_delta.serialize())
            self.log("Swapping gamedelta, old =", old_delta.version_no,
                     "new =", self.current_delta.version_no)

    def tick(self, dt):
        set_simulation_time(self.simulation_time)
        self.entity_mgr.update(dt)

        # Process event queue
        if self.event_queue:
            processed_events = 0
            for event in sorted(self.event_queue, key=lambda event: event.timestamp):
                if event.timestamp > self.event_simulation_time:
                    # not ready yet
                    break
                self.log("Processing event", event)
                processed_events += 1
                if not event.can_execute(event.client, self.entity_mgr):
                    self.log("ERROR: Client is not allowed to execute event!")
                    self.current_delta.declined_events.add(event.uuid)
                    # for entity_uuid in event.get_affected_entities():
                        # entity = self.entity_mgr.find(entity_uuid)
                        # if entity:
                            # self.current_delta.changed_entities.add(entity)
                    continue
                    
                # Adjust corrections
                event.update(event.client, self.entity_mgr)
                changed_entities = event.execute(event.client, self.entity_mgr)
                # for entity in changed_entities:
                    # self.current_delta.changed_entities.add(entity)
                self.current_delta.confirmed_events.add(event)

            self.event_queue = self.event_queue[processed_events:]

        self.swap_delta()

        for client in self.clients:
            if not client.ready:
                continue

            # Check for lost client
            if (fast_time() - client.last_confirmation_time) > self.MAX_QUIET_TIME:
                self.log("No response from client, disconnecting client.")
                self.clients.remove(client)
                break

            if len(client.unconfirmed_deltas) > self.MAX_UNCONFIRMED_DELTAS:
                self.log(
                    "Client is missing more than 10 responses for deltas, disconnecting client.")
                self.clients.remove(client)
                break

            # Re-Send unconfirmed deltas
            last_resend_attempt = client.last_resend_attempt
            if fast_time() - last_resend_attempt > self.DELTA_RESEND_RATE:
                client.last_resend_attempt = fast_time()
                for version_no in client.unconfirmed_deltas:
                    for delta in reversed(self.deltas):
                        if delta["version_no"] == version_no:
                            if fast_time() - delta["timestamp"] > self.DELTA_RESEND_WAIT:
                                self.log("Re-Sending unconfirmed delta",
                                         delta["version_no"])
                                client.send(Message.MID_GAMEDELTA,
                                            delta, reliable=False)
                            else:
                                self.log(
                                    "Delta unconfirmed, but wait resend time mot reached yet")
                            break

            # Send new deltas
            deltas_to_send = []
            for delta in reversed(self.deltas):
                if delta["version_no"] > client.last_sent_delta:
                    deltas_to_send.append(delta)

            for delta in reversed(deltas_to_send):
                self.log("Sending initial delta", delta[
                         "version_no"], "to client")
                client.send(Message.MID_GAMEDELTA, delta, reliable=False)
                client.last_sent_delta = max(
                    client.last_sent_delta, delta["version_no"])
                client.unconfirmed_deltas.add(delta["version_no"])

    def log(self, *args):
        # print(*args)
        add_to_packet_log(" ".join([str(i) for i in args]))
