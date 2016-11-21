
from panda3d.core import *
from direct.gui.DirectGui import DirectFrame
from direct.gui.OnscreenText import OnscreenText

from networked_types import NetworkedFloat

VISUAL_DEBUGGING = False

class Component(object):

    def __init__(self, entity):
        self._entity = entity

    def clear(self):
        self._entity = None

    def get_changed_vars(self, keep_update_status=False):
        return {}

    def load_delta(self, json, timestamp, index):
        pass

    def interpolate(self, timestamp, index):
        pass

    def correct_delta(self, json, index):
        pass

    def save_delta(self, index):
        pass

    def has_changes(self):
        return False

    def correct(self, dt):
        pass

class TransformComponent(Component):

    def __init__(self, entity):
        super().__init__(entity)
        self.pos_x = NetworkedFloat(0, "pos_x")
        self.pos_y = NetworkedFloat(0, "pos_y")
        self.rotation = 0

        if VISUAL_DEBUGGING:
            self._frames_debug = []
            self._position_indicators = []

            ps = 0.04
            self._start_interpolator = DirectFrame(pos=(0, 0, 0), frameColor=(0, 1, 0, 0.8), hpr=(0, 0, 45), frameSize=(-ps, ps, ps, -ps))
            self._end_interpolator = DirectFrame(pos=(0, 0, 0), frameColor=(0, 0, 1, 0.8), hpr=(0, 0, 45), frameSize=(-ps, ps, ps, -ps))
            self._start_interpolator.hide()
            self._end_interpolator.hide()

            self._server_pos = DirectFrame(pos=(0, 0, 0), frameColor=(1, 1, 1, 0.5), frameSize=(-0.05, 0.05, -0.05, 0.05))
            self._server_pos.hide()

    def serialize(self):
        return {"pos": (self.pos_x.v, self.pos_y.v), "rotation": self.rotation}

    def load(self, data):
        self.pos_x.v = data["pos"][0]
        self.pos_y.v = data["pos"][1]

        self.rotation = data["rotation"]
        self.pos_x.updated()
        self.pos_y.updated()

    def get_changed_vars(self, keep_update_status=False):
        result = {}
        if self.pos_x.changed:
            result["pos_x"] = self.pos_x.v
            if not keep_update_status:
                self.pos_x.updated()
        
        if self.pos_y.changed:
            result["pos_y"] = self.pos_y.v
            if not keep_update_status:
                self.pos_y.updated()
        return result

    def load_delta(self, json, timestamp, index):
        self.pos_x.add_point(json["pos_x"], timestamp, index)
        self.pos_y.add_point(json["pos_y"], timestamp, index)

        if VISUAL_DEBUGGING:
            ps = 0.015
            self._frames_debug.append((
                DirectFrame(pos=(json["pos_x"], 0, json["pos_y"]), frameColor=(1, 0, 0, 0.45), frameSize=(-ps, ps, ps, -ps)),
                OnscreenText(pos=(json["pos_x"], json["pos_y"] + 0.06), text="{:6.1f}".format(timestamp * 1000.0), fg=(1, 0.3, 1, 1), scale=0.05)
            ))

    def correct_delta(self, json, index):
        self.pos_x.correct_delta(json["pos_x"], index)
        self.pos_y.correct_delta(json["pos_y"], index)

    def correct(self, dt):
        self.pos_x.correct(dt)
        self.pos_y.correct(dt)

        if VISUAL_DEBUGGING:
            self._server_pos.show()
            self._server_pos.set_pos(self.pos_x._value + self.pos_x._required_correction, 0, self.pos_y._value + self.pos_y._required_correction)

    def save_delta(self, index):
        self.pos_x.save_delta(index)
        self.pos_y.save_delta(index)

    def interpolate(self, timestamp, index):
        pxs, pxe = self.pos_x.interpolate(timestamp, index)
        pys, pye = self.pos_y.interpolate(timestamp, index)

        if VISUAL_DEBUGGING:
            if pxs == 0 or pys == 0:
                self._start_interpolator.hide()
                self._end_interpolator.hide()
                return


            self._start_interpolator.show()
            self._end_interpolator.show()
            self._start_interpolator.set_pos(pxs, 0, pys)
            self._end_interpolator.set_pos(pxe, 0, pye)

            px, py = self.pos_x.v, self.pos_y.v
            if not self._position_indicators or abs(px - self._position_indicators[-1].get_x()) > 0.0005 or abs(py - self._position_indicators[-1].get_y()) > 0.0005:
                ps = 0.01
                self._position_indicators.append(
                    DirectFrame(pos=(px, 0, py), frameColor=(0, 1, 1, 0.45), frameSize=(-ps, ps, ps, -ps))
                )


    def has_changes(self):
        return self.pos_x.changed or self.pos_y.changed

class VelocityComponent(Component):

    def __init__(self, entity):
        super().__init__(entity)
        self.velocity = Vec2(0)

    def serialize(self):
        return {"velocity": (self.velocity.x, self.velocity.y)}

    def load(self, data):
        self.velocity = Vec2(*data["velocity"])


class InteractableComponent(Component):

    def __init__(self, entity):
        super().__init__(entity)
        self.radius = 5
        self.text = "Interact [E]"

    def serialize(self):
        return {"radius": self.radius, "text": self.text}

    def load(self, data):
        self.radius = data["radius"]
        self.text = data["text"]


class DrawComponent(Component):

    def __init__(self, entity):
        self.mesh = None

    def serialize(self):
        return {"mesh": [i for i in self.mesh["frameColor"]]}

    def load(self, data):
        if not self.mesh:
            try:
                self.mesh = DirectFrame(frameColor=Vec4(
                    *data["mesh"]), frameSize=(-0.05, 0.05, -0.05, 0.05))
            except:
                # In server
                self.mesh = {"frameColor": data["mesh"]}

    def clear(self):
        super().clear()
        if self.mesh and isinstance(self.mesh, DirectFrame):
            self.mesh.remove_node()
            del self.mesh
