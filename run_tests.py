
import panda3d
import panda3d.core

import p3d_ecs.native.p3d_ecs as p3d_ecs

open("test-output.txt", "w").close()

# input("Press enter to start: ")

ALL_TESTSUITES = ["uuid", "entityref", "generic", "parent_child", "collectors", "perftest", "entity"]


# TODO: Make this configurable with command line args
suites_to_run = ALL_TESTSUITES[:]
#suites_to_run = ["generic", "serialization"]
suites_to_run.remove("perftest")

for suite in suites_to_run:
    print("\n\n" + "=" * 10, "Running test suite", suite, "=" * 10, "\n")
    getattr(p3d_ecs, "testsuite_" + suite)()


print("Tests finished.")
