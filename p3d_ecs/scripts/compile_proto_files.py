
import os
import sys
import glob
import subprocess
from os.path import realpath, abspath, join, dirname

this_dir = abspath(realpath(dirname(__file__)))
base_dir = abspath(join(this_dir, "..", ".."))
proto_src_dir = abspath(join(base_dir, "p3d_ecs", "proto"))


if sys.platform == "win32":
    proto_dir = abspath(join(base_dir, "thirdparty", "protobuf", "protobuf-built"))
    protoc = abspath(join(proto_dir, "protoc"))
else:
    protoc = "protoc"

print("PROTOC=", protoc)

src_dir = join(base_dir, "p3d_ecs", "native", "source")
dest = join(src_dir, "proto_gen")

os.chdir(proto_src_dir)

for f in glob.glob("**/*.proto"):
    abspath = join(proto_src_dir, f)
    # print(f)
    subprocess.call([protoc, "-I=" + src_dir, "--cpp_out=" + dest, "--proto_path=" + proto_src_dir, abspath])
