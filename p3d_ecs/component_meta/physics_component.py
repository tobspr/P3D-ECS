
from p3d_ecs.meta.component_properties import *

class PhysicsComponent(object):

    # Public
    mass = FloatProperty(
        name="Mass",
        init_with=10.0,
        generate_setter=True
    )

    hpr = VectorProperty(
        name="Rotation",
        dimensions=2,
        generate_setter=True
    )
