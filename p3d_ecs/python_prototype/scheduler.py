

class ScheduledQueue(object):

    def __init__(self, schedule_delay=0):
        self._delay = schedule_delay
        self._entries = []

    def append(self, index, data):
        self._entries.append((index + self._delay, data))

    def take_entries(self, index):
        matching = []
        for entry in sorted(self._entries, key=lambda entry: entry[0]):
            if entry[0] <= index:
                matching.append(entry)
                yield entry[1]
        for entry in matching:
            self._entries.remove(entry)
