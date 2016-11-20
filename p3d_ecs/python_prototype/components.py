
from panda3d.core import *
from direct.gui.DirectGui import DirectFrame

from networked_types import NetworkedFloat

class Component(object):

    def __init__(self, entity):
        self._entity = entity

    def clear(self):
        self._entity = None

    def get_changed_vars(self):
        return {}

class TransformComponent(Component):

    def __init__(self, entity):
        super().__init__(entity)
        self.pos_x = NetworkedFloat(0)
        self.pos_y = NetworkedFloat(0)
        self.rotation = 0

    def serialize(self):
        return {"pos": (self.pos_x.v, self.pos_y.v), "rotation": self.rotation}

    def load(self, data):
        self.pos_x.v = data["pos"][0]
        self.pos_y.v = data["pos"][1]
        self.rotation = data["rotation"]
        self.pos_x.updated()
        self.pos_y.updated()

    def get_changed_vars(self):
        result = {}
        if self.pos_x.changed:
            result["pos_x"] = self.pos_x.v
            self.pos_x.updated()
        if self.pos_y.changed:
            result["pos_y"] = self.pos_y.v
            self.pos_y.updated()
        return result


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
