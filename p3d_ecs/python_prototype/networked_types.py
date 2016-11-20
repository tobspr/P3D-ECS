
MAX_INTERPOLATION_DELAY = 1.0


class NetworkedFloat(object):

    def __init__(self, val):
        self._value = val
        self._server_value = val
        self._changed = False
        self._points = []

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
    
    def add_point(self, data, timestamp):
        # print("FLOAT: point at ", timestamp, " -> ", data)
        self._points.append((timestamp, data))
        # self._value = data

    def interpolate(self, timestamp):
        if not self._points:
            return
        past_point = (-1e10, None)
        future_point = (1e10, None)

        for t, point in self._points:
            if t > past_point[0] and t <= timestamp:
                past_point = (t, point)
            if t < future_point[0] and t >= timestamp:
                future_point = (t, point)

        if past_point[1] == None or future_point[1] == None:
            # print("No interpolation with just 1 point possible!")
            # print(timestamp, past_point, future_point, self._points)

            self._value = past_point[1] if past_point[1] is not None else future_point[1]
            return

        if future_point[0] - past_point[0] > MAX_INTERPOLATION_DELAY:
            # Interpolation delay too big. just use first point 
            self._value = past_point[1]
            return

        # print("Interpolate between", past_point, "and", future_point)
        lerp = max(0, min(1, (timestamp - past_point[0]) / max(1e-10, future_point[0] - past_point[0])))
        self._value = past_point[1] * (1 - lerp) + future_point[1] * lerp

