"""

Processes all meta files

"""

from __future__ import print_function

import importlib
import inspect

from os.path import isfile, isdir, join, realpath, dirname
from os import listdir

from p3d_ecs.meta.component_properties import BaseProperty, InternalProperty

THIS_DIR = realpath(dirname(__file__))

def generate_additional_component_includes(members):
    includes = set()
    for member_name, member_type in members:
        if hasattr(member_type, "includes"):
            for inc in member_type.includes:
                includes.add(inc)

    out = "// AUTOGEN:: aditional includes\n"
    for include in includes:
        out += '#include ' + include + '\n'
    out += "\n"
    if not includes:
        return ""
    return out

def generate_component_member_attributes(members, indent="  "):
    out = indent + "// AUTOGEN:: internal members\n"

    for member_name, member_type in members:
        internal_name = "_" + member_name
        out += indent + member_type.DATA_TYPE + " " + internal_name + ";\n"
        
    return out

def generate_component_accessors(members, indent="  "):
    out = indent + "// AUTOGEN:: accessors\n"
    def ref_type(member, const=True):
        const_pref = "const " if const else ""
        if member.PASS_AS_REF:
            return "const " + member.DATA_TYPE + "&"
        return member.DATA_TYPE

    for member_name, member_type in members:
        if member_type.generate_getter:
            out += indent + "inline " + ref_type(member_type) + " get_" + member_name + "() const { "
            out += "return _" + member_name + "; }\n"
        if member_type.generate_setter:
            out += indent + "inline void set_" + member_name + "(" + ref_type(member_type) + " val) { "
            out += "_" + member_name + " = val; }\n"
        
        if member_type.generate_setter or member_type.generate_getter:
            out += "\n"

    return out

def generate_component_constructor_init(members, cls_name, indent="  "):
    out = indent + "// AUTOGEN:: constructor\n"
    out += indent + "inline " + cls_name + "(Entity* entity) : Component(entity)\n"
    for member_name, member_type in members:
        if member_type.init_with is not None:
            out += indent + "  , _" + member_name + "(" + str(member_type.init_with) + ")\n"
    out += indent + "  {};\n\n"
    return out

def generate_cpp_header_component_definition(name, members, index):
    cls_name = name + "Meta"
    indent = "  "

    out = "#pragma once\n\n"
    out += "// AUTOGENERATED. DO NOT EDIT!\n"
    out += "// Instead modify the component meta file.\n\n"
    out += "#ifndef P3D_ECS_" + name.upper() + "_METACLS_H\n"
    out += "#define P3D_ECS_" + name.upper() + "_METACLS_H\n\n"
    out += '#include "config_module.h"\n'
    out += '#include "component.h"\n'
    out += '#include "perf_utility.h"\n'
    out += '#include "memory_pool.h"\n'
    out += '#include "luse.h"\n'
    out += "\n"
    out += generate_additional_component_includes(members)
    out += "class Entity;\n"
    out += "class EntityManager;\n"    
    out += "class YAMLSerializer;\n"    
    out += "class " + name + ";\n\n"
    out += "class " + cls_name + " : public Component {\n"

    out += indent + "using superclass = " + cls_name + ";\n\n"
    out += indent + "public:\n"
    out += 2 * indent + "DEFINE_COMPONENT_BASE();\n\n"

    out += generate_component_accessors(members, 2 * indent)
    out += "\n"
    out += 2 * indent + "// AUTOGEN:: serialization\n"
    out += 2 * indent + "void serialize(YAMLSerializer* serializer) const;\n"
    out += "\n"

    out += indent + "protected:\n"
    out += generate_component_constructor_init(members, cls_name, 2 * indent)
    out += generate_component_member_attributes(members, 2 * indent)

    out += "};\n"

    out += "#endif\n"
    return out

def generate_cpp_file_component_definition(header, name, members, index):
    cls_name = name + "Meta"
    out = "\n"
    out += '#include "' + header + '"\n\n'
    out += '#include "serialization.h"\n\n'
    
    out += "IMPLEMENT_COMPONENT_BASE(" + cls_name + ", " + str(index) + "u);\n"
    out += "\n"

    out += "void " + cls_name + "::serialize(YAMLSerializer* serializer) const {\n"
    indent = "  "
    for member_name, member_type in members:
        out += indent + "// Serialize " + member_name + "\n"
        if isinstance(member_type, InternalProperty) and member_type.serializer is not None:
            out += indent + "   " + member_type.serializer + "(serializer, \"" + member_name + "\", _" + member_name + ");\n"
        else:
            out += indent + "if (" + member_type.check_for_default("_" + member_name) + ") {\n"
            out += indent + "    serializer->serialize_prop(\"" + member_name + "\", _" + member_name + ");\n"
            out += indent + "}\n"

    out += "\n}\n"

    return out

def parse_template(source, defines):
    with open(source) as handle:
        content = handle.read()
    for key, val in defines.items():
        content = content.replace("${" + key + "}", str(val))
    return content

def process_meta_component(py_file, filename, index):
    py_name = filename.replace(".py", "")
    camelcase_name = py_name.title().replace("_", "")
    print("Processing component", camelcase_name)

    module = importlib.import_module(("p3d_ecs.component_meta.{}".format(py_name)))
    if not hasattr(module, camelcase_name):
        print("ERROR: Component class not found in", py_file)
        return False
    
    component_class = getattr(module, camelcase_name)
    pred = lambda m: isinstance(m, BaseProperty)
    members = inspect.getmembers(component_class, pred)
    for i, (member_name, member_type) in enumerate(members):
        member_type.member_id = i

    cpp_name = py_name + "_meta.cpp"
    header_name = py_name + "_meta.h"

    header_code = generate_cpp_header_component_definition(camelcase_name, members, index)
    cpp_code = generate_cpp_file_component_definition(header_name, camelcase_name, members, index)

    source_dir = join(THIS_DIR, "p3d_ecs", "native", "source")
    destination = join(source_dir, "component_meta")

    with open(join(destination, header_name), "w") as handle:
        handle.write(header_code)

    with open(join(destination, cpp_name), "w") as handle:
        handle.write(cpp_code)

    impl_dir = join(source_dir, "component_impl")
    impl_header = join(impl_dir, py_name + ".h")
    impl_cpp = join(impl_dir, py_name + ".cpp")

    if not isfile(impl_cpp):
        print(" -> Adding cpp template for", py_file)
        with open(impl_cpp, "w") as handle:
            handle.write('\n#include "' + py_name + '.h"\n\n')

    if not isfile(impl_header):
        print(" -> Adding header template for", py_file)
        template_params = {
            "CLASSNAME_UPPER": py_name.upper(),
            "META_HEADER": py_name + "_meta.h",
            "CLASSNAME": camelcase_name,
        }
        prefab = join(THIS_DIR, "p3d_ecs", "script_templates", "new_component.templ.h")
        content = parse_template(prefab, template_params)
        with open(impl_header, "w") as handle:
            handle.write(content)
        
    return py_name + ".h"


def process_component_metafiles():
    meta_path = join(THIS_DIR, "p3d_ecs", "component_meta")
    component_index = 1
    available_components = []

    # Notice: we use sorted to ensure we always have the same order
    for f in sorted(listdir(meta_path)):
        abspath = join(meta_path, f)
        if isfile(abspath) and f.endswith("_component.py"):
            component_header = process_meta_component(abspath, f, component_index)
            component_index += 1     
            available_components.append(component_header)

    # Write the combined all_components header
    all_components_file = join(THIS_DIR, "p3d_ecs", "native", "source", "all_components.h")
    with open(all_components_file, "w") as handle:
        handle.write("#pragma once\n")
        handle.write("#ifndef P3D_ECS_ALL_COMPONENTS_H\n")
        handle.write("#define P3D_ECS_ALL_COMPONENTS_H\n")
        handle.write("// AUTOGENERATED. DO NOT EDIT!\n\n")
        for header in available_components:
            handle.write('#include "' + header + '"\n')
        handle.write("\n#endif\n")

    print("Done!")


if __name__ == "__main__":
    process_component_metafiles()
