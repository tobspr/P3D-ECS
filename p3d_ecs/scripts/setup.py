# TODO: Write real setup script

print("Importing panda3d")
import panda3d
import subprocess

import os, sys
from os.path import isfile, isdir, realpath, dirname, join

THIS_DIR = realpath(dirname(__file__)) or "."

start_cwd = os.getcwd() or "."

print("Processing meta files")
os.chdir(THIS_DIR)
subprocess.call([sys.executable, "process_meta_files.py"])
os.chdir(join(THIS_DIR, "../../p3d_ecs/native/"))

print("Updating module builder ..")
subprocess.call([sys.executable, "update_module_builder.py"])

print("Compiling native code")
subprocess.call([sys.executable, "build.py"])
os.chdir(start_cwd)
sys.exit(0)
