# TODO: Write real setup script

import os, sys
os.chdir("p3d_ecs/native/")
os.system('"' + sys.executable + '" update_module_builder.py')
os.system('"' + sys.executable + '" build.py')
