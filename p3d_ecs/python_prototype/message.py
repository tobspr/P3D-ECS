
import json

class Message(object):
    DIVIDER = "%$%"
    CHANNEL_UNRELIABLE = 0
    CHANNEL_RELIABLE = 1

    MID_CONNECT = "connect"
    MID_INIT_GAMESTATE = "init_gamestate"
    MID_CLIENT_READY = "client_ready"

    MID_GAMEDELTA = "gamedelta"
    MID_CONFIRM_DELTA = "confirm_delta"

    MID_REQUEST_EVENT = "request_event"

    @classmethod
    def make(cls, mid, data):
        return mid + cls.DIVIDER + json.dumps(data)

    @classmethod
    def parse(cls, data):
        parts = data.split(cls.DIVIDER)
        assert len(parts) == 2
        # print("Parsing message:", parts)
        return parts[0], json.loads(parts[1])
