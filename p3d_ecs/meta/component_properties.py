
from panda3d.core import Vec3, Mat4

class BaseProperty(object):
    def __init__(self, **kwargs):
        self.name = kwargs.get("name", "<unnamed>")
        self.generate_getter = kwargs.get("generate_getter", True)
        self.generate_setter = kwargs.get("generate_setter", False)
        self.editor_visible = kwargs.get("editor_visible", True)
        self.init_with = kwargs.get("init_with", None)

class FloatProperty(BaseProperty):
    DATA_TYPE = "float"
    PASS_AS_REF = False

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # prevent unitialized members
        self.init_with = kwargs.get("init_with", 0.0)

    def check_for_default(self, identifier):
        return "std::abs(" + identifier + " - (float)" + str(self.init_with) + ") > 1e-10"

class BoolProperty(BaseProperty):
    DATA_TYPE = "bool"
    PASS_AS_REF = False

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # prevent unitialized members
        self.init_with = kwargs.get("init_with", "false")

    def check_for_default(self, identifier):
        return identifier + " != " + str(self.init_with)
        
class VectorProperty(BaseProperty):
    PASS_AS_REF = True

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.dimensions = kwargs["dimensions"]
        self.component_type = kwargs.get("component_type", "float")

    @property
    def DATA_TYPE(self):
        prefix = {
            "float": "f",
            "int": "i"
        }[self.component_type]
        return "LVecBase" + str(self.dimensions) + prefix

    def check_for_default(self, identifier):
        return identifier + " != " + self.DATA_TYPE + "()"

class Mat4Property(BaseProperty):
    DATA_TYPE = "LMatrix4f"
    PASS_AS_REF = True

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def check_for_default(self, identifier):
        return identifier + " != LMatrix4f()"

class EntityRefProperty(BaseProperty):
    PASS_AS_REF = True
    DATA_TYPE = "EntityRef"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.includes = ['"entity_ref.h"']

    def check_for_default(self, identifier):
        return "!" + identifier + ".is_empty()"
        
class InternalProperty(BaseProperty):
    PASS_AS_REF = True

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.data_type = kwargs["data_type"]
        self.includes = kwargs.get("includes", [])
        self.serializer = kwargs.get("serializer", None)
        self.deserializer = kwargs.get("deserializer", None)

    @property
    def DATA_TYPE(self):
        return self.data_type

    def check_for_default(self, identifier):
        return "true" # TODO
    