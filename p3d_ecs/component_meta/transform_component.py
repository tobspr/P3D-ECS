
from p3d_ecs.meta.component_properties import *

class TransformComponent(object):

    # Public
    pos = VectorProperty(
        name="Position",
        dimensions=3
    )

    hpr = VectorProperty(
        name="Rotation",
        dimensions=3
    )

    scale = VectorProperty(
        name="Scale",
        dimensions=3,
        init_with="1, 1, 1"
    )

    # Private
    is_dirty = BoolProperty(
        editor_visible=False,
        init_with="false"
    )

    mat = Mat4Property(
        init_with="LMatrix4f::ident_mat()",
        editor_visible=False,
        serialize=False
    )

    #parent = EntityRefProperty()
    parent = EntityPtrProperty()

    children = ContainerProperty(
        value_type="Entity*"
    )