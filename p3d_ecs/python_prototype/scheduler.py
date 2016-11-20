

class ScheduledQueue(object):

    def __init__(self, schedule_delay_ms=0):
        self._delay = schedule_delay_ms / 1000.0
        self._entries = []

    def append(self, timestamp, data):
        self._entries.append((timestamp + self._delay, data))

    def take_entries(self, timestamp):
        matching = []
        for entry in sorted(self._entries, key=lambda entry: entry[0]):
            if entry[0] <= timestamp:
                matching.append(entry)
                yield entry[1]
        for entry in matching:
            self._entries.remove(entry)
