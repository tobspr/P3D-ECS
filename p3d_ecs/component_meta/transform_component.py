
from p3d_ecs.meta.component_properties import *

class TransformComponent(object):
    ID = 1

    # Next serialization ID: 4

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

    matrix = VectorProperty(
        serialization_id=1,
        dimensions=4,
        is_matrix=True,
        init_with="LMatrix4f::ident_mat()",
        editor_visible=False,
        serialize=False
    )

    absolute_matrix = VectorProperty(
        dimensions=4,
        is_matrix=True,
        init_with="LMatrix4f::ident_mat()",
        editor_visible=False,
        serialize=False,
        generate_getter=False
    )

    abs_matrix_is_dirty = BoolProperty(
        editor_visible=False,
        init_with="false"
    )

    parent = EntityPtrProperty(
        serialization_id=2
    )

    children = ContainerProperty(
        serialization_id=3,
        value_type="Entity*"
    )
