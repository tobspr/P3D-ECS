
import config


class NetworkedFloat(object):

    def __init__(self, val, name):
        self._value = val
        self._changed = False
        self._points = [(0, self._value)]
        self._name = name
        self._values_at_simtime = {}

        self._current_correction = 0
        self._required_correction = 0

    @property
    def v(self):
        return self._value + self._current_correction

    @v.setter
    def v(self, value):
        self._value = value - self._current_correction
        self._changed = True

        # Override any interpolation, since the client set the values manually.
        self._points = [(0, self._value)]

    @property
    def changed(self):
        return self._changed

    def updated(self):
        self._changed = False
    
    def add_point(self, data, timestamp, index):
        self._points.append((timestamp, data))
        self._points = list(sorted(self._points, key=lambda entry: entry[0]))

    def _save_value(self, value, index):
        self._value = value

    def save_delta(self, index):
        self._values_at_simtime[index] = self._value

    def correct(self, dt):
        delta = self._required_correction - self._current_correction
        delta *= dt
        delta /= config.PREDICTION_CORRECTION
        self._current_correction += delta

    def correct_delta(self, data, index):
        if index not in self._values_at_simtime:
            print("Warning, tried to correct delta", index, "but no value recorded")
            self._required_correction = 0
            return
        predicted_value = self._values_at_simtime[index]
        self._required_correction = data - predicted_value
        # if abs(self._required_correction) > 1e-5:
        #     print("PREDICTION ERROR! required correction =", self._required_correction)
        

    def interpolate(self, timestamp, index):
        # We can only ever interpolate if we have at least 2 points
        if len(self._points) < 2:
            self._save_value(self._points[-1][1], index)
            return 0, 0

        past_point = (-1e10, None)
        future_point = (1e10, None)

        relevant_points = []
        
        for t, point in self._points:
            if t > past_point[0] and t <= timestamp:
                past_point = (t, point)
            if t < future_point[0] and t >= timestamp:
                future_point = (t, point)
            
            # if t < timestamp - 5.0 * config.MAX_INTERPOLATION_DELAY:
                # print("Droppping point", t, point)
            #     pass
            # else:
                # relevant_points.append((t, point))
            
        # self._points = relevant_points
            
        if future_point[1] is None or (future_point[1] is not None and abs(future_point[0] - timestamp) > config.MAX_INTERPOLATION_DELAY):
            self._save_value(self._points[-1][1], index)
            return 0, 0


        if past_point[1] is not None and abs(past_point[0] - timestamp) > config.MAX_INTERPOLATION_DELAY:
            # Out of 
            # if self._name == "pos_x":
                # print("!WARN! For {} the past point delay is too big.".format(self._name))
            # past_point = (-1e10, None)
            # Move past point closer to avoid bad interpolation
            # print("Past point is too far away, moving to valid delta")
            # print("   t before = {:3.3f}, after = {:3.3f} ".format(past_point[0], future_point[0] - 1.0 / config.UPDATE_RATE))
            past_point = (future_point[0] - 1.0 / config.UPDATE_RATE, past_point[1])

        if past_point[1] == None:
            # No future data, cannot interpolate
            # TODO: Extrapolate?!
            print("No past point found. can not interpolate!")
            self._save_value(self._points[-1][1], index)
            return 0, 0

        # if past_point[1] == None:
        #     last_update = future_point[0] - 1.0 / config.UPDATE_RATE
        #     if last_update < timestamp:
        #         past_point = [future_point[0] - 1.0 / config.UPDATE_RATE]
        #     else:
        #         # Ok, the last update is still in the future, no interpolation
        #         return 0, 0


        lerp = max(0, min(1, (timestamp - past_point[0]) / max(1e-10, future_point[0] - past_point[0])))

        # if self._name == "pos_x":
        #     print("Interpolate {} between [{:3.3f}] {:3.3f} and [{:3.3f}] {:3.3f}, lerp = {:1.4f}, t = {:3.3f}".format(self._name, *past_point, *future_point, lerp, timestamp))
        #     print(" -> {:2.5f}".format(self._value))
        interpolated = past_point[1] * (1 - lerp) + future_point[1] * lerp
        
        self._save_value(interpolated, index)
        return past_point[1], future_point[1]
