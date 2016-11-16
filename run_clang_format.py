"""

Runs clang format over all cpp files

"""

import os
import glob

ignore = [
    ".templ.h",
    "CMakeFiles",
    "interrogate_"
]
extensions = "cpp h I i".split()
files = []

for ext in extensions:
    for f in glob.glob("p3d_ecs/**/*." + ext, recursive=True):
        files.append(f)

for f in files:
    for txt in ignore:
        if txt in f or f in txt:
            break
    else:
        print(f)
        os.system("clang-format -i -style=file \"" + f + "\"")
