
import time
import random

from message import Message
from entity_mgr import EntityManager
from components import *
from game_state import GameState, GameDelta

from libenet import add_to_packet_log, set_simulation_time

class ServerLogic(object):

    def __init__(self, server_app):
        self.server_app = server_app
        self.entity_mgr = EntityManager()
        self.deltas = []
        self.current_delta = GameDelta(1)
        self.clients = set()
        self.start_time = time.time()
        self.last_delta_sent = 0

    @property
    def simulation_time(self):
        return time.time() - self.start_time

    def load(self):
        self.log("ServerLogic::load")

        # TODO: Load from file
        door = self.entity_mgr.new_entity()
        transform = door.new_component(TransformComponent)
        draw = door.new_component(DrawComponent)
        interact = door.new_component(InteractableComponent)
        interact.radius = 0.3

        transform.pos.set(0.7, 0.1)
        transform.rotation = 45
        draw.load({"mesh": (0.2, 0.6, 0.3, 1.0)})
        self.current_delta.changed_entities.add(door)

        # Stress test
        for i in range(100):
            entity = self.entity_mgr.new_entity()
            transform = entity.new_component(TransformComponent)
            transform.pos.x = random.random()
            transform.pos.y = random.random()

    def handle_new_client(self, client):
        self.clients.add(client)

    def handle_message(self, client, message_id, data):
        if message_id == Message.MID_CONNECT:

            # Spawn player entitiy
            player_entity = self.entity_mgr.new_entity()
            transform = player_entity.new_component(TransformComponent)
            draw = player_entity.new_component(DrawComponent)
            transform.pos.set(0.2, 0.2)
            draw.load({"mesh": (1.0, 0.6, 0.2, 1.0)})

            # Make sure we have the newest game state
            self.swap_delta()

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
            self.log("Client", client, "is now ready, and at version", client.last_sent_delta)
            client.ready = True
            client.last_confirmation_time = time.time()

        elif message_id == Message.MID_CONFIRM_DELTA:

            version_no = data["version_no"]
            if version_no in client.unconfirmed_deltas:
                self.log("Client confirmed game delta", version_no)
                client.unconfirmed_deltas.remove(version_no)
                client.last_confirmation_time = time.time()

    def swap_delta(self):
        needs_delta = (time.time() - self.last_delta_sent) > 0.2
        if needs_delta or not self.current_delta.empty:
            self.last_delta_sent = time.time()
            old_delta = self.current_delta
            self.current_delta = GameDelta(old_delta.version_no + 1)
            self.deltas.append(old_delta.serialize())
            self.log("Swapping gamedelta, old =", old_delta.version_no, "new =", self.current_delta.version_no)


    def tick(self, dt):
        set_simulation_time(self.simulation_time)
        self.entity_mgr.update(dt)
        self.swap_delta()

        for client in self.clients:
            if not client.ready:
                continue

            # Check for lost client
            if (time.time() - client.last_confirmation_time) > 5.0:
                self.log("No response from client, disconnecting client.")
                self.clients.remove(client)
                break


            if len(client.unconfirmed_deltas) > 10:
                self.log("Client is missing more than 10 responses for deltas, disconnecting client.")
                self.clients.remove(client)
                break


            # Re-Send unconfirmed deltas
            wait_resend_time = 0.2
            resend_interval = 0.2

            last_resend_attempt = client.last_resend_attempt
            if time.time() - last_resend_attempt > resend_interval:
                client.last_resend_attempt = time.time()
                for version_no in client.unconfirmed_deltas:
                    for delta in reversed(self.deltas):
                        if delta["version_no"] == version_no:
                            if time.time() - delta["timestamp"] > wait_resend_time:
                                self.log("Re-Sending unconfirmed delta", delta["version_no"])
                                client.send(Message.MID_GAMEDELTA, delta, reliable=False)
                            else:
                                self.log("Delta unconfirmed, but wait resend time mot reached yet")
                            break

            # Send new deltas
            deltas_to_send = []
            for delta in reversed(self.deltas):
                if delta["version_no"] > client.last_sent_delta:
                    deltas_to_send.append(delta)

            for delta in reversed(deltas_to_send):
                self.log("Sending initial delta", delta["version_no"], "to client")
                client.send(Message.MID_GAMEDELTA, delta, reliable=False)
                client.last_sent_delta = max(client.last_sent_delta, delta["version_no"])
                client.unconfirmed_deltas.add(delta["version_no"])

    def log(self, *args):
        # print(*args)
        add_to_packet_log(" ".join([str(i) for i in args]))
