
import json

class Message(object):
    DIVIDER = "%$%"
    CHANNEL_UNRELIABLE = 0
    CHANNEL_RELIABLE = 1

    MID_CONNECT = "connect"
    MID_CONNECT_ACCEPT = "accept_connection"
    MID_CONSTRUCT_ENTITY = "construct_entity"
    MID_ASSIGN_PLAYER_ENTITY = "assign_player_entity"
    MID_GAMESTATE = "gamestate"
    MID_GAMEDELTA = "gamedelta"
    MID_TRIGGER_EVENT = "trigger_event"

    @classmethod
    def make(cls, mid, data):
        return mid + cls.DIVIDER + json.dumps(data)

    @classmethod
    def parse(cls, data):
        parts = data.split(cls.DIVIDER)
        assert len(parts) == 2
        print("Parsing message:", parts)
        return parts[0], json.loads(parts[1])
