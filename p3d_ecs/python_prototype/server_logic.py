
import time
import random

from message import Message
from entity_mgr import EntityManager
from components import *
from game_state import GameDelta
from events import GameEvent
from scheduler import ScheduledQueue
from libenet import add_to_packet_log, set_simulation_time, fast_time

import config

class ServerLogic(object):

    def __init__(self, server_app):
        self.server_app = server_app
        self.entity_mgr = EntityManager()
        self.clients = set()
        self.start_time = fast_time()

        self.current_delta = GameDelta(1000)
        self.entity_mgr.creation_callback = lambda entity: self.current_delta.on_entity_created(entity)

        self.deltas = []
        self.event_queue = ScheduledQueue(config.EVENT_DELAY)
        self.swap_delta()

    @property
    def simulation_time(self):
        return fast_time() - self.start_time

    def log(self, *args):
        print(*args)
        add_to_packet_log(" ".join([str(i) for i in args]))

    def error(self, *args):
        # print("ERROR", *args)
        self.log("ERROR", *args)

    def swap_delta(self):
        for entity in self.entity_mgr.entities:
            if entity.has_changes() and entity not in self.current_delta.new_entities:
                self.current_delta.changed_entities.add(entity)
        # if self.current_delta.changed_entities or self.current_delta.new_entities:
            # self.log("New delta, version =", self.current_delta.version_no,
            #         "changed =", len(self.current_delta.changed_entities), "new=", len(self.current_delta.new_entities))
        self.current_delta.index = self.entity_mgr.simulation_index + config.DELTA_DELAY
        self.current_delta.sent_timestamp = self.simulation_time
        self.current_delta.store_serialization()
        self.deltas.append(self.current_delta)
        self.current_delta = GameDelta(self.current_delta.version_no + 1)

        for client in self.clients:
            if client.ready:
                self.send_delta(client, self.deltas[-1])
                client.last_sent_delta = self.deltas[-1].version_no
                client.unconfirmed_deltas.add(self.deltas[-1].version_no)

    def send_delta(self, client, delta):
        client.send(Message.MID_GAMEDELTA, delta.serialized, reliable=False)

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
                        "index": self.entity_mgr.simulation_index,
                        "version_no": self.current_delta.version_no - 1
                        }, reliable=True)
            client.last_sent_delta = self.current_delta.version_no - 1

        elif message_id == Message.MID_CLIENT_READY:

            self.log("Client", client, "is now ready")
            client.ready = True
            client.send(Message.MID_SYNC_SIMULATION_INDEX, {
                "index": self.entity_mgr.simulation_index,
                "timestamp": self.simulation_time
            }, reliable=True)

        elif message_id == Message.MID_REQUEST_EVENT:
            
            event = GameEvent.from_serialized(data)
            self.event_queue.append(event.index + config.EVENT_DELAY, (client, event))

        elif message_id == Message.MID_CONFIRM_DELTA:

            version = data["version_no"]
            if version in client.unconfirmed_deltas:
                client.unconfirmed_deltas.remove(version)
                # self.log("Client confirmed delta", version)

    def tick(self, dt):
        set_simulation_time(self.simulation_time)
        self.entity_mgr.update(dt)

        # Process events
        for client, event in self.event_queue.take_entries(self.entity_mgr.simulation_index):

            if not event.can_execute(client, self.entity_mgr):
                response_data = []
                for entity_uuid in event.get_affected_entities(client, self.entity_mgr):
                    entity = self.entity_mgr.find(entity_uuid)
                    assert entity
                    response_data.append(entity.serialize())
            
                self.error("Event execution denied. TODO.")        
                client.send(Message.MID_REJECT_EVENT, {"event": event.uuid, "affected_entities": response_data}, reliable=True)
                continue

            response_data = []
            for entity_uuid in event.get_affected_entities(client, self.entity_mgr):
                entity = self.entity_mgr.find(entity_uuid)
                assert entity
                response_data.append(entity.serialize_changes(keep_update_status=True))
        

            event.execute(client, self.entity_mgr)

            # TODO: Send only variables which are predicted
            client.send(Message.MID_CONFIRM_EVENT, {"event": event.uuid, "affected_entities": response_data}, reliable=True)


        any_changes = False
        for entity in self.entity_mgr.entities:
            if entity.has_changes():
                any_changes = True

        for client in self.clients:
            if client.ready:

                if len(client.unconfirmed_deltas) > 10:
                    self.error("Client", client, "missing more than 10 deltas, TODO: force resync")
                    self.error("Missing deltas:", client.unconfirmed_deltas)
                    client.unconfirmed_deltas = set()
                    continue

                if (fast_time() - client.last_resend_attempt) > config.DELTA_RESEND_RATE / 1000.0:
                    client.last_resend_attempt = fast_time()
                    for version_no in client.unconfirmed_deltas:
                        delta = None
                        for delta_ in reversed(self.deltas):
                            if delta_.version_no == version_no:
                                delta = delta_
                                break
                        assert delta
                        if (fast_time() - delta.sent_timestamp) > config.DELTA_RESEND_MIN_DUR / 1000.0:
                            self.log("Resending delta", delta.version_no, "because of missing ack")
                            self.send_delta(client, delta)

        duration = self.simulation_time - self.deltas[-1].sent_timestamp
        if (duration > (1.0 / config.MAX_DELTAS_PER_FRAME)) and any_changes:
            self.swap_delta()
        elif duration > config.AUTO_DELTA_RATE / 1000.0:
            self.swap_delta()
