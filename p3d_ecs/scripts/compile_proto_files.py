
import os
import glob
import subprocess
from os.path import realpath, abspath, join, dirname

this_dir = abspath(realpath(dirname(__file__)))
base_dir = abspath(join(this_dir, "..", ".."))
proto_dir = abspath(join(base_dir, "thirdparty", "protobuf-built"))
proto_src_dir = abspath(join(base_dir, "p3d_ecs", "Proto"))

protoc = abspath(join(proto_dir, "protoc"))
print("PROTOC=", protoc)

src_dir = join(base_dir, "p3d_ecs", "native", "source")
dest = join(src_dir, "proto_gen")

os.chdir(proto_src_dir)

for f in glob.glob("**/*.proto", recursive=True):
    abspath = join(proto_src_dir, f)
    # print(f)
    subprocess.call([protoc, "-I=" + src_dir, "--cpp_out=" + dest, "--proto_path=" + proto_src_dir, abspath])
