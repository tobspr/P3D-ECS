
import panda3d
import panda3d.core

import p3d_ecs.native.p3d_ecs as p3d_ecs

open("test-output.txt", "w").close()

# input("Press enter to start: ")

# TODO: Make this configurable with command line args
suites_to_run = ["uuid", "entityref", "generic", "parent_child", "collectors", "perftest"]

for suite in suites_to_run:
    print("Running test suite", suite)
    getattr(p3d_ecs, "testsuite_" + suite)()


print("Tests finished.")
