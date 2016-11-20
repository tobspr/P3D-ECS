


class NetworkedFloat(object):

    def __init__(self, val):
        self._value = val
        self._server_value = val
        self._changed = False

    @property
    def v(self):
        return self._value

    @v.setter
    def v(self, value):
        self._value = value
        self._changed = True

    @property
    def changed(self):
        return self._changed


    def updated(self):
        self._changed = False