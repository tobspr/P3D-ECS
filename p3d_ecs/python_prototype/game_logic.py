
import random

from panda3d.core import Vec4

from entity_mgr import EntityManager
from components import *
from message import *
from game_state import *
from events import *
from systems import *

from direct.gui.DirectFrame import DirectFrame

class GameLogic(object):

    def __init__(self, connector):
        self.manager = EntityManager()
        self.connector = connector
        self.manager.creation_callback = self.on_new_entity
        self.new_entities = []
        self.state_deltas = []
        self.waiting_deltas = []
        self.predicted_events = []

        self.velocity_system = self.manager.new_system(ForceSystem)


    def on_new_entity(self, entity):
        pass

    @property
    def render_simulation_time(self):
        # We always render all other entities 100ms in the back
        return time.time() - 100.0 / 1000.0

    def load(self):

        # Load "world"
        # TODO: Deserialize from file
        door = self.manager.new_entity()
        transform = door.new_component(TransformComponent)
        draw = door.new_component(DrawComponent)
        interact = door.new_component(InteractableComponent)
        interact.radius = 0.3

        transform.pos.set(0.7, 0.1)
        transform.rotation = 45
        draw.load({"mesh": (0.2, 0.6, 0.3, 1.0)})

    def tick(self, dt):

        self.manager.update(dt)
        # newest_version = len(self.state_deltas)  # todo. clear up old states
        # for client in self.connector.get_owned_clients():
        #     old_version = client.version_no
        #     if old_version != newest_version:
        #         self.log("Sending deltas from", old_version + 1,
        #                  "to", newest_version, "to update", client)
        #         for i in range(old_version, newest_version):
        #             self.log("Sending delta", i + 1)
        #             delta = self.state_deltas[i]
        #             msg = Message.make(Message.MID_GAMEDELTA, {
        #                                "version_no": i + 1,
        #                                "action_class": delta.__class__.__name__,
        #                                "data": delta.serialize()})
        #             # client.send(msg)
        #         client.version_no = newest_version

        actions = []
        for entity in self.manager.entities:
            action = GameActionModifyEntity.from_entity(entity)
            msg = Message.make(Message.MID_GAMEDELTA, {
                                "version_no": len(self.state_deltas),
                                "action_class": action.__class__.__name__,
                                "data": action.serialize()})
            actions.append(msg)

        for client in self.connector.get_owned_clients():
            for action in actions:
                client.send(action)

        # Playback deltas
        while self.waiting_deltas:
            client, data = self.waiting_deltas[0]
            timestamp = data["data"]["_timestamp"]
            if timestamp > self.render_simulation_time:
                break
            self.waiting_deltas.pop(0)

            action_class = globals()[data["action_class"]]
            instance = action_class(data["data"])
            # predicted = False
            client.version_no = data["version_no"]
            # self.log("Applying delta", client.version_no)
            # if action_class == GameActionExecuteEvent:
            #     # Check for predicted events
            #     for i, predicted_event in enumerate(self.predicted_events):
            #         if predicted_event.uuid == instance.event_uuid:
            #             self.log("Predicted event", predicted_event.uuid, "got confirmed")
            #             if i == 0:
            #                 # All fine, we expected this
            #                 self.predicted_events.pop(0)
            #                 predicted = True
            #                 break

            # if not predicted:
            self.log("Executing new event: ", instance)
            instance.execute(self.manager)
            self.state_deltas.append(instance)


    def handle_message(self, client, message_id, data):

        # TODO: Server/Client only messages check
        if message_id == Message.MID_CONNECT:
            self.log("connect accepted, sending conn_accepted")

            self.connector.send_message(
                client, Message.make(Message.MID_CONNECT_ACCEPT, {"player_id": client.cid}))
            player_entity_uuid = self.make_player_entity(client)

            # TODO: Instead of sending all deltas, construct a new game state
            self.connector.send_message(
                client, Message.make(Message.MID_ASSIGN_PLAYER_ENTITY, {
                                     "uuid": player_entity_uuid})
            )

        elif message_id == Message.MID_CONNECT_ACCEPT:

            player_id = data["player_id"]
            client.cid = player_id
            self.log(
                "LOGIC: Connection accepted, setting player id to:", client.cid)

        elif message_id == Message.MID_ASSIGN_PLAYER_ENTITY:
            uuid = data["uuid"]
            client.assigned_entity = data["uuid"]

        elif message_id == Message.MID_GAMESTATE:

            self.log("Recieved gamestate, reverting to that state")
            self.manager.clear()
            for entity in data["entities"]:
                self.manager.load_entity(entity)
            client.version_no = data["version_no"]
            self.log("Loaded", len(data), "entities")

        elif message_id == Message.MID_GAMEDELTA:

            self.waiting_deltas.append((client, data))

        elif message_id == Message.MID_ASSIGN_PLAYER_ENTITY:

            entity = self.manager.find(data["uuid"])
            self.connector.assign_player_entity(client, entity)

        elif message_id == Message.MID_TRIGGER_EVENT:

            # TODO: Go back in time and check if the event was okay to execute.

            event_time = data["timestamp"]
            # Like, go_to(event_time), check_event, go_to(current_time)

            event_class = data["event_class"]
            event_data = data["event_data"]

            event_instance = globals()[event_class](event_data)
            event_instance.uuid = data["event_uuid"]
            event_instance.execute(self.manager)

            # action = GameActionExecuteEvent.from_event(event_instance)
            # action.data["_timestamp"] = event_time
            # self.state_deltas.append(action)

        else:
            self.log("UNKOWN MESSAGE ID:", message_id)

    def trigger_event(self, client, event):
        self.log("Trigger event", event.__class__.__name__)
        if self.connector.is_server:
            pass
        else:
            event.uuid = random.randint(10**9, 10**10 - 1)
            data = event.serialize()
            self.log("Broadcast event to server:", data)
            self.predicted_events.append(event)
            msg_data = {
                "event_class": event.__class__.__name__,
                "event_data": data,
                "event_uuid": event.uuid,
                "timestamp": time.time()
            }
            msg = Message.make(Message.MID_TRIGGER_EVENT, msg_data)
            client.send(msg)
            # client.push_message(msg)


    def log(self, *args):
        # print("LOGIC::", self.connector.prefix, *args)
        pass

    def make_player_entity(self, client):
        entity = self.manager.new_entity()
        transform = entity.new_component(TransformComponent)
        velocity = entity.new_component(VelocityComponent)
        draw = entity.new_component(DrawComponent)
        draw.load({"mesh": (0.2, 0.6, 1.0, 1.0)})
        transform.pos.x = random.random() - 0.5
        transform.pos.y = random.random() - 0.5
        return entity.uuid
