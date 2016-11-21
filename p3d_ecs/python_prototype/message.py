
import json

class Message(object):

    DIVIDER = "%$%"
    CHANNEL_UNRELIABLE = 0
    CHANNEL_RELIABLE = 1
    CHANNEL_TIMESYNC = 2

    MID_CONNECT = "connect"

    MID_SYNC_TIME = "sync_time"
    MID_SYNC_TIME_RESP = "sync_time_response"
    MID_SYNC_SIMULATION_INDEX = "sync_simulation_index"

    MID_CLIENT_INITIAL_SYNC = "initial_sync"
    MID_INIT_GAMESTATE = "init_gamestate"
    MID_CLIENT_READY = "client_ready"

    MID_GAMEDELTA = "gamedelta"
    MID_CONFIRM_DELTA = "confirm_delta"

    MID_REQUEST_EVENT = "request_event"

    MID_REJECT_EVENT = "reject_event"
    MID_CONFIRM_EVENT = "confirm_event"

    @classmethod
    def make(cls, mid, data):
        return mid + cls.DIVIDER + json.dumps(data)

    @classmethod
    def parse(cls, data):
        parts = data.split(cls.DIVIDER)
        assert len(parts) == 2
        # print("Parsing message:", parts)
        return parts[0], json.loads(parts[1])
