
from p3d_ecs.meta.component_properties import *

class PhysicsComponent(object):
    ID = 2
    
    # Next serialization ID: 3

    # Public
    mass = FloatProperty(
        serialization_id=1,
        name="Mass",
        init_with=10.0,
        generate_setter=True
    )

    is_static = BoolProperty(
        serialization_id=2,
        name="Static",
        init_with="false",
        generate_setter=True
    )
