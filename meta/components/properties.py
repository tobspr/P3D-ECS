
from panda3d.core import Vec3, Mat4

class BaseProperty(object):
    def __init__(self, **kwargs):
        self.identifier = kwargs["identifier"]
        self.name = kwargs.get("name", "<unnamed>")
        self.default = kwargs.get("default", self.DEFAULT)
        self.generate_getter = kwargs.get("generate_getter", True)
        self.generate_setter = kwargs.get("generate_setter", True)
        self.editor_visible = kwargs.get("editor_visible", True)

class FloatProperty(BaseProperty):
    DEFAULT = 0.0
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

class BoolProperty(BaseProperty):
    DEFAULT = False
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

class Vec3Property(BaseProperty):
    DEFAULT = Vec3(0)
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

class Mat4Property(BaseProperty):
    DEFAULT = Mat4.identMat()
    def __init__(self, **kwargs):
        super().__init__(**kwargs)

class InternalProperty(BaseProperty):
    DEFAULT = ""
    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.data_type = kwargs["data_type"]