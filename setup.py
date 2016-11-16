# TODO: Write real setup script

print("Importing panda3d")
import panda3d

import os, sys
print("Starting to build ...")
os.chdir("./p3d_ecs/native/")
# os.system('"' + sys.executable + '" update_module_builder.py')
os.system('"' + sys.executable + '" build.py')
os.chdir(os.path.abspath(os.path.dirname(__file__)) or ".")
sys.exit(0)
