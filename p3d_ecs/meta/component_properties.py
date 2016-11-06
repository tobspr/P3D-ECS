
from panda3d.core import Vec3, Mat4

class BaseProperty(object):
    def __init__(self, **kwargs):
        self.name = kwargs.get("name", "<unnamed>")
        self.generate_getter = kwargs.get("generate_getter", True)
        self.generate_setter = kwargs.get("generate_setter", False)
        self.editor_visible = kwargs.get("editor_visible", True)
        self.init_with = kwargs.get("init_with", self.TYPE_DEFAULT)

    def check_for_default(self, identifier):
        # Default check for default implementation
        return "!(" + self.compare_to(identifier, str(self.init_with), convert_type=True) + ")"

    def compare_to(self, a, b, convert_type=False):
        # Default compare implementation
        if convert_type:
            b = self.DATA_TYPE + "(" + b + ")"
        return "{} == {}".format(a, b)


class FloatProperty(BaseProperty):
    DATA_TYPE = "float"
    PASS_AS_REF = False
    TYPE_DEFAULT = "0.0"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # prevent unitialized members
        self.init_with = kwargs.get("init_with", 0.0)

    def compare_to(self, a, b, convert_type=False):
        return "compare_float({}, {})".format(a, b)

class BoolProperty(BaseProperty):
    DATA_TYPE = "bool"
    PASS_AS_REF = False
    TYPE_DEFAULT = "false"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        # prevent unitialized members
        self.init_with = kwargs.get("init_with", "false")

class VectorProperty(BaseProperty):
    PASS_AS_REF = True
    TYPE_DEFAULT = ""

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

    def compare_to(self, a, b, convert_type=False):
        if convert_type:
            b = self.DATA_TYPE + "(" + b + ")"
        return "{}.almost_equal({})".format(a, b)

class Mat4Property(BaseProperty): # todo: generic matrix type (should be straightforward)
    DATA_TYPE = "LMatrix4f"
    PASS_AS_REF = True
    TYPE_DEFAULT = ""

    def __init__(self, **kwargs):
        super().__init__(**kwargs)


class EntityRefProperty(BaseProperty):
    PASS_AS_REF = True
    DATA_TYPE = "EntityRef"
    TYPE_DEFAULT = "Entity::EMPTY_ID"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.includes = ['"entity_ref.h"']

    def check_for_default(self, identifier):
        return "!" + identifier + ".is_empty()"

    def compare_to(self, a, b, convert_type=False):
        # convert_type is ignored
        return "{}.get_id() == {}.get_id()".format(a, b)

class EntityPtrProperty(BaseProperty):
    PASS_AS_REF = False
    DATA_TYPE = "Entity*"
    TYPE_DEFAULT = "nullptr"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)

    def compare_to(self, a, b, convert_type=False):
        return "{} == {}".format(a, b)

class ContainerProperty(BaseProperty):
    PASS_AS_REF = True
    TYPE_DEFAULT = ""

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.value_type = kwargs.get("value_type")
        self.includes = kwargs.get("includes", []) + ["<vector>"]

    @property
    def DATA_TYPE(self):
        return "std::vector<" + self.value_type + ">"

    def check_for_default(self, identifier):
        return "!" + identifier + ".empty()"

    def compare_to(self, a, b, convert_type=False):
        # convert type is ignored
        return "vector_set_intersection({}, {})".format(a, b)
    
class InternalProperty(BaseProperty):
    PASS_AS_REF = True
    TYPE_DEFAULT = "/* ??? */"

    def __init__(self, **kwargs):
        super().__init__(**kwargs)
        self.data_type = kwargs["data_type"]
        self.includes = kwargs.get("includes", [])
        self.serializer = kwargs.get("serializer", None)
        self.deserializer = kwargs.get("deserializer", None)

    @property
    def DATA_TYPE(self):
        return self.data_type

    # def check_for_default(self, identifier):
    #     return "true" # TODO
    