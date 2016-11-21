
from direct.gui.OnscreenText import OnscreenText

from components import *
from events import *


class RenderSystem(object):
    REQUIRES = [TransformComponent, DrawComponent]
    CATCH_EVENTS = []
    FIXED_STEP = False

    def __init__(self, mgr):
        self._manager = mgr

    def process(self, entities, dt):
        for entity in entities:
            draw = entity.get_component(DrawComponent)
            transform = entity.get_component(TransformComponent)

            if draw.mesh:
                draw.mesh.set_pos(transform.pos_x.v, 0, transform.pos_y.v)
                draw.mesh.set_r(transform.rotation)

class PhysicsSystem(object):
    REQUIRES = [TransformComponent, VelocityComponent]
    FIXED_STEP = True

    def __init__(self, mgr):
        self._manager = mgr

    def process(self, entities, dt):
        for entity in entities:
            c_velocity = entity.get_component(VelocityComponent)
            c_transform = entity.get_component(TransformComponent)

            vel = c_velocity.velocity * dt
            if vel.length() > 0.0:
                c_transform.pos_x.v += vel.x
                c_transform.pos_y.v += vel.y

class InteractSystem(object):
    REQUIRES = [InteractableComponent, TransformComponent]

    def __init__(self, mgr, player=None):
        self._manager = mgr
        self.player = player
        self._tmp_texts = []
        self._available_interacts = []

    def process(self, entities, dt):
        for text in self._tmp_texts:
            text.remove_node()
            del text
        self._tmp_texts = []
        self._available_interacts = []

        if self.player and self.player.has_component(TransformComponent):
            player_pos = self.player.get_component(TransformComponent).pos
            for entity in entities:
                pos = entity.get_component(TransformComponent).pos
                interact = entity.get_component(InteractableComponent)
                dist = (pos - player_pos).length()
                if dist < interact.radius:
                    text = OnscreenText(interact.text, pos=(
                        pos.x, 0, pos.y), align=TextNode.A_center, fg=Vec4(1, 1, 1, 1), bg=Vec4(0, 0, 0, 0.5))
                    self._tmp_texts.append(text)
                    self._available_interacts.append(entity)
