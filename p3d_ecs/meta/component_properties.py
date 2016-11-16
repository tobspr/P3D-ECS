
from panda3d.core import Vec3, Mat4

class BaseProperty(object):
    CUSTOM_SERIALIZATION = False
    NEEDS_DEFAULT_MEMBER = False
    PROTO_INC = None
    PASS_AS_REF = False
    IS_SIMPLE_TYPE = True
    NEEDS_FILLIN = False

    def __init__(self, **kwargs):
        self.name = kwargs.get("name", "<unnamed>")
        self.generate_getter = kwargs.get("generate_getter", True)
        self.generate_setter = kwargs.get("generate_setter", False)
        self.editor_visible = kwargs.get("editor_visible", True)
        self.init_with = kwargs.get("init_with", self.TYPE_DEFAULT)
        self.serialization_id = kwargs.get("serialization_id", None)

    def check_for_default(self, identifier):
        # Default check for default implementation
        return "!(" + self.compare_to(identifier, str(self.init_with), convert_type=True) + ")"

    def compare_to(self, a, b, convert_type=False):
        # Default compare implementation
        if convert_type:
            b = self.DATA_TYPE + "(" + b + ")"
        return "{} == {}".format(a, b)

    def serialize_to_proto(self, proto_obj, name):
        return "{}->set_{}(_{})".format(proto_obj, name, name)

    def deserialize_from_proto(self, proto_obj, name):
        return "_{} = {}.{}()".format(name, proto_obj, name)

    def generate_default_object(self):
        if self.IS_SIMPLE_TYPE:
            return str(self.init_with)
        return self.DATA_TYPE + "(" + str(self.init_with) + ")"

    @property
    def CUSTOM_DESERIALIZATION(self):
        return self.CUSTOM_SERIALIZATION

class BasePrimitiveType(BaseProperty):
    PROTO_INC = "google/protobuf/wrappers.proto"


    def serialize_to_proto(self, proto_obj, name):
        return "{}->mutable_{}()->set_value(_{})".format(proto_obj, name, name)

    def deserialize_from_proto(self, proto_obj, name):
        return "_{} = {}.{}().value()".format(name, proto_obj, name)

class FloatProperty(BasePrimitiveType):
    DATA_TYPE = "float"
    TYPE_DEFAULT = 0.0
    PROTO_TYPE = "google.protobuf.FloatValue"

    def compare_to(self, a, b, convert_type=False):
        return "::p3d_ecs::utility::float_equals({}, {})".format(a, b)

class BoolProperty(BasePrimitiveType):
    DATA_TYPE = "bool"
    TYPE_DEFAULT = "false"
    PROTO_TYPE = "google.protobuf.BoolValue"
    PROTO_INC = "google/protobuf/wrappers.proto"

class VectorProperty(BaseProperty):
    PASS_AS_REF = True
    TYPE_DEFAULT = "0"
    PROTO_INC = "component_types/luse.proto"
    CUSTOM_SERIALIZATION = True
    NEEDS_DEFAULT_MEMBER = True
    IS_SIMPLE_TYPE = False

    def __init__(self, **kwargs):
        BaseProperty.__init__(self, **kwargs)
        self.dimensions = kwargs["dimensions"]
        self.component_type = kwargs.get("component_type", "float")
        self.is_matrix = kwargs.get("is_matrix", False)

    @property
    def DATA_TYPE(self):
        prefix = {
            "float": "f",
            "int": "i"
        }[self.component_type]
        return ("LMatrix" if self.is_matrix else "LVecBase") + str(self.dimensions) + prefix

    @property
    def PROTO_TYPE(self):
        return "p3d_ecs.proto." + self.DATA_TYPE

    def compare_to(self, a, b, convert_type=False):
        if convert_type:
            b = self.DATA_TYPE + "(" + b + ")"
        return "{}.almost_equal({})".format(a, b)

    def check_for_default(self, identifier):
        return "!" + identifier + ".almost_equal(" + identifier + "_DEFAULT)"

    def serialize_to_proto_custom(self, proto_obj, name, indent):
        out = indent + "if (" + self.check_for_default("_" + name) + ") {\n"
        out += indent + "  serialize_" + self.DATA_TYPE + "(_" + name + ", " + proto_obj + "->mutable_" + name + "());\n"
        out += indent + "}\n"
        return out

    def deserialize_from_proto_custom(self, proto_obj, name, indent):
        out = indent + "if (!" + proto_obj + ".has_" + name + "()) {\n"
        out += indent + "  _" + name + " = _" + name + "_DEFAULT;\n"
        out += indent + "} else {\n"
        out += indent + "  deserialize_" + self.DATA_TYPE + "(_" + name + ", " + proto_obj + "." + name + "());\n"
        out += indent + "}\n"
        return out

# class EntityRefProperty(BaseProperty):
#     PASS_AS_REF = True
#     DATA_TYPE = "EntityRef"
#     TYPE_DEFAULT = "nullptr"
#     PROTO_TYPE = "bytes"
#     PROTO_INC = "component_types/entity_ref.proto"
#     IS_SIMPLE_TYPE = False

#     def check_for_default(self, identifier):
#         return "!" + identifier + ".is_empty()"

#     def compare_to(self, a, b, convert_type=False):
#         # convert_type is ignored
#         return "{}.get_id() == {}.get_id()".format(a, b)

class EntityPtrProperty(BaseProperty):
    DATA_TYPE = "Entity*"
    TYPE_DEFAULT = "nullptr"
    PROTO_TYPE = "bytes"
    IS_SIMPLE_TYPE = True
    CUSTOM_DESERIALIZATION = True
    NEEDS_FILLIN = True
    FILLIN_STORAGE_TYPE = "UUID"

    def __init__(self, **kwargs):
        BaseProperty.__init__(self, **kwargs)

    def compare_to(self, a, b, convert_type=False):
        return "{} == {}".format(a, b)

    def serialize_to_proto(self, proto_obj, name):
        return proto_obj + "->set_" + name + "(_" + name + "->get_uuid().c_str(), UUID::uuid_length)"

    def deserialize_from_proto_custom(self, proto_obj, name, indent):
        return indent + "deserialize_Entity(_" + name + ", _" + name + "_FILLIN_CACHE, " + proto_obj + "." + name + "());\n"

    def fillin_ptrs(self, name, indent):
        cache_name = "_" + name + "_FILLIN_CACHE"
        return indent + "fillin_Entity(mgr, " + cache_name + ", _" + name + ");\n"


class ContainerProperty(BaseProperty):
    TYPE_DEFAULT = ""
    CUSTOM_SERIALIZATION = True
    IS_SIMPLE_TYPE = False
    PASS_AS_REF = True

    def __init__(self, **kwargs):
        BaseProperty.__init__(self, **kwargs)
        self.value_type = kwargs.get("value_type")
        self.includes = kwargs.get("includes", []) + ["<vector>"]

    @property
    def NEEDS_FILLIN(self):
        return self.value_type in ["Entity*"]

    @property
    def FILLIN_STORAGE_TYPE(self):
        return "std::vector<" + {"Entity*": "UUID"}[self.value_type] + ">"

    @property
    def DATA_TYPE(self):
        return "std::vector<" + self.value_type + ">"

    @property
    def PROTO_TYPE(self):
        return "repeated " + self.internal_proto_type[1]

    @property
    def internal_proto_type(self):
        return {
            "Entity*": (None, "bytes")
        }[self.value_type]

    def compare_to(self, a, b, convert_type=False):
        # convert type is ignored
        return "::p3d_ecs::performance::compare_flat_sets({}, {})".format(a, b)

    def serialize_to_proto_custom(self, proto_obj, name, indent):
        out = ""
        out += indent + "for (auto& elem : _" + name + ") {\n"
        if self.value_type == "Entity*":
            out += indent + "   proto_obj->add_" + name + "(elem->get_uuid().c_str(), UUID::uuid_length);\n"
        out += indent + "}\n"
        return out

    def deserialize_from_proto_custom(self, proto_obj, name, indent):
        out = indent + "deserialize_VectorOf" + self.value_type.replace("*", "") + "("
        out += "_" + name + ", _" + name + "_FILLIN_CACHE, " + proto_obj + "." + name + "());\n"
        return out

    def fillin_ptrs(self, name, indent):
        cache_name = "_" + name + "_FILLIN_CACHE"
        out = ""
        # out += indent + "for (auto& entry : " + cache_name + ") {\n"
        if self.value_type == "Entity*":
            out += indent + "fillin_VectorOf" + self.value_type.replace("*", "") + "(mgr, "
            out += cache_name + ", _" + name + ");\n"
        # out += indent + "}\n"
        # out += indent + cache_name + ".clear();\n"
        return out
