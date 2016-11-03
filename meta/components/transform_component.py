
from panda3d.core import Vec3, Mat4

from .properties import *

class TransformComponent(object):

    # Public
    pos = Vec3Property(
        identifier="pos",
        name="Position",
        default=Vec3(0),
        generate_setter=False
    )

    hpr = Vec3Property(
        identifier="hpr",
        name="Rotation",
        default=Vec3(0),
        generate_setter=False
    )

    scale = Vec3Property(
        identifier="scale",
        name="Scale",
        default=Vec3(1),
        generate_setter=False
    )

    # Private
    dirty = BoolProperty(
        identifier="is_dirty",
        default=False,
        editor_visible=False,
        generate_setter=False
    )

    mat = Mat4Property(
        identifier="mat",
        default=Mat4.identMat(),
    )

    parent = InternalProperty(
        identifier="parent",
        data_type="Entity*",
        generate_setter=False,
    )
     
    children = InternalProperty(
        identifier="children",
        data_type="std::vector<Entity*>",
        generate_setter=False
    )
