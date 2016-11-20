
import time
import random

from message import Message
from entity_mgr import EntityManager
from components import *
from game_state import GameDelta
from events import GameEvent
from scheduler import ScheduledQueue
from libenet import add_to_packet_log, set_simulation_time, fast_time


class ServerLogic(object):

    # Send deltas at least every x milliseconds
    AUTO_DELTA_RATE = 1000


    # Do not send deltas too frequently
    MAX_DELTAS_PER_FRAME = 20 # Max 10 deltas per frame

    # Execute events x milliseconds after being executed on the client
    EVENT_DELAY = 50

    # Execute deltas on client side x milliseconds after being sent 
    DELTA_DELAY = 2.0 / MAX_DELTAS_PER_FRAME * 1000.0


    def __init__(self, server_app):
        self.server_app = server_app
        self.entity_mgr = EntityManager()
        self.clients = set()
        self.start_time = fast_time()

        self.current_delta = GameDelta(1000)
        self.entity_mgr.creation_callback = lambda entity: self.current_delta.on_entity_created(entity)

        self.deltas = []
        self.event_queue = ScheduledQueue(self.EVENT_DELAY)
        self.swap_delta()

    @property
    def simulation_time(self):
        return fast_time() - self.start_time

    def log(self, *args):
        print(*args)
        add_to_packet_log(" ".join([str(i) for i in args]))

    def error(self, *args):
        print("ERROR", *args)
        self.log("ERROR", *args)

    def swap_delta(self):
        for entity in self.entity_mgr.entities:
            if entity.has_changes() and entity not in self.current_delta.new_entities:
                self.current_delta.changed_entities.add(entity)
                
        # if self.current_delta.changed_entities or self.current_delta.new_entities:
            # self.log("Swapped delta, version =", self.current_delta.version_no,
            #         "changed =", len(self.current_delta.changed_entities), "new=", len(self.current_delta.new_entities))
        self.current_delta.sent_timestamp = self.simulation_time
        self.current_delta.timestamp = self.simulation_time + self.DELTA_DELAY / 1000.0
        self.current_delta.store_serialization()
        self.deltas.append(self.current_delta)

        self.current_delta = GameDelta(self.current_delta.version_no + 1)

        for client in self.clients:
            if client.ready:
                client.send(Message.MID_GAMEDELTA, self.deltas[-1].serialized)
                client.last_confirmed_delta = self.deltas[-1].version_no # todo

    def load(self):
        self.log("ServerLogic::load")

        # TODO: Load from file
        # door = self.entity_mgr.new_entity()
        # transform = door.new_component(TransformComponent)
        # draw = door.new_component(DrawComponent)
        # interact = door.new_component(InteractableComponent)
        # interact.radius = 0.3

        # transform.pos_x.v = 0.7
        # transform.pos_y.v = 0.1
        # transform.rotation = 45
        # draw.load({"mesh": (0.2, 0.6, 0.3, 1.0)})


    def handle_new_client(self, client):
        self.clients.add(client)

    def handle_message(self, client, message_id, data):
        if message_id == Message.MID_SYNC_TIME:
            client.send(
                Message.MID_SYNC_TIME_RESP, {
                    "id": data["id"],
                    "simulation_time": self.simulation_time
                },
                reliable=False, channel=Message.CHANNEL_TIMESYNC)

        elif message_id == Message.MID_CLIENT_INITIAL_SYNC:

            if client.ready:
                self.error(
                    "Client is already synced, but rerequested sync! Bad client?")
                return

            # Spawn player entity
            player_entity = self.entity_mgr.new_entity()
            transform = player_entity.new_component(TransformComponent)
            draw = player_entity.new_component(DrawComponent)
            vel = player_entity.new_component(VelocityComponent)
            transform.pos_x.v = random.random() * 2.0 - 1.0
            transform.pos_y.v = random.random() * 2.0 - 1.0
            draw.load({"mesh": (1.0, 0.6, 0.2, 1.0)})

            client.entity_uuid = player_entity.uuid

            self.swap_delta()

            # Send accept connect, and also include all entities
            client.send(Message.MID_INIT_GAMESTATE, {
                        "entities": [entity.serialize() for entity in self.entity_mgr.entities],
                        "player_entity": player_entity.uuid,
                        "simulation_time": self.simulation_time,
                        "version_no": self.current_delta.version_no - 1
                        })
            client.last_confirmed_delta = self.current_delta.version_no - 1

        elif message_id == Message.MID_CLIENT_READY:

            self.log("Client", client, "is now ready")
            client.ready = True

        elif message_id == Message.MID_REQUEST_EVENT:
            
            event = GameEvent.from_serialized(data)
            time_to_execute = event.timestamp
            self.event_queue.append(time_to_execute, (client, event))

    def tick(self, dt):
        set_simulation_time(self.simulation_time)
        self.entity_mgr.update(dt)

        # Process events
        for client, event in self.event_queue.take_entries(self.simulation_time):
            self.log("Processing event", client, event)

            if not event.can_execute(client, self.entity_mgr):
                self.error("Event execution denied. TODO.")
                continue

            event.execute(client, self.entity_mgr)

        any_changes = False
        for entity in self.entity_mgr.entities:
            if entity.has_changes():
                any_changes = True

        for client in self.clients:
            if client.ready:
                # Send unsent deltas
                last_delta = client.last_confirmed_delta
                for delta in self.deltas:
                    if delta.version_no > last_delta:
                        client.send(Message.MID_GAMEDELTA, delta.serialized)
                client.last_confirmed_delta = self.deltas[-1].version_no

        duration = self.simulation_time - self.deltas[-1].sent_timestamp
        if (duration > (1.0 / self.MAX_DELTAS_PER_FRAME)) and any_changes:
            self.swap_delta()
        elif duration > self.AUTO_DELTA_RATE / 1000.0:
            self.swap_delta()

