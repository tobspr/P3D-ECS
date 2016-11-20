

import time
from statistics import mean
from message import Message

from libenet import fast_time

class TimeSynchronization(object):

    MAX_TRAVEL_TIME = 2.0

    def __init__(self, client):
        self._client = client
        self._offset = None
        self._confirmed_syncs = []
        self._sequence_no = 0
        self._last_sync = fast_time()
        self._sent_syncs = {}

    @property
    def simulation_time(self):
        return self.simulation_time_at_point(fast_time())

    def simulation_time_at_point(self, t):
        if not self._offset:
            return 0.0
        return t + self._offset
        
    @property
    def sync_interval(self):
        if len(self._confirmed_syncs) < 4:
            return 0.1
        return 20.0

    @property
    def time_information_sufficient(self):
        return len(self._confirmed_syncs) >= 2

    def update(self, vclient):
        if fast_time() - self._last_sync > self.sync_interval:
            self._send_sync_packet(vclient)
            self._last_sync = fast_time()
            
    def _send_sync_packet(self, vclient):
        vclient.send(Message.MID_SYNC_TIME, {"id": self._sequence_no}, reliable=False, channel=Message.CHANNEL_TIMESYNC)
        self._sent_syncs[self._sequence_no] = fast_time()
        self._sequence_no += 1

    def handle_time_sync_response(self, vclient, data):
        sequence_no = data["id"]
        sim_time = data["simulation_time"]
        sent_time = self._sent_syncs[sequence_no]
        del self._sent_syncs[sequence_no]
        recv_time = fast_time()
        travel_time = (recv_time - sent_time) / 2
        if travel_time > self.MAX_TRAVEL_TIME:
            self._client.log("Got time sync, but outside of max travel time (max=", self.MAX_TRAVEL_TIME, "actual=", travel_time, ")")
            return
        new_time = sim_time + travel_time
        # self._client.log("timesync, travel time =", travel_time, "new time=", new_time)
        self._confirmed_syncs.append((new_time - fast_time(), travel_time))
        self._average_syncs()

    def _average_syncs(self):
        """ Averages the last 5 syncs to find out the simulation time """
        if self.time_information_sufficient:
            best_results = sorted(self._confirmed_syncs, key=lambda v: v[1])
            avg_deltas = []
            for delta, travel_time in best_results[0:5]:
                avg_deltas.append(delta)
            avg_delta = mean(avg_deltas)
            self._offset = avg_delta
            self._client.log("Synchronized time, delta to local time =", avg_delta, "simtime=", self.simulation_time)

