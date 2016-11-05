
from p3d_ecs.meta.component_properties import *

class PhysicsComponent(object):

    # Public
    mass = FloatProperty(
        name="Mass",
        init_with=10.0,
        generate_setter=True
    )

    static = BoolProperty(
        name="Static",
        init_with="false",
        generate_setter=True
    )
