# TODO: Write real setup script

import os, sys
print("Starting to build ...")
os.chdir("./p3d_ecs/native/")
os.system('"' + sys.executable + '" update_module_builder.py')
os.system('"' + sys.executable + '" build.py')
os.chdir(os.path.dirname(__file__))
sys.exit(0)
