"""

Main Testrunner

"""

from __future__ import print_function

import inspect
import panda3d
import os
import sys
import panda3d.core

import p3d_ecs.native.p3d_ecs as p3d_ecs

open("test-output.txt", "w").close()
try:
    os.remove("test-errors.txt")
except:
    pass

ALL_TESTSUITES = [i for i, _ in inspect.getmembers(p3d_ecs) if i.startswith("testsuite_")]
print("Suites:", "\n" + '\n'.join(ALL_TESTSUITES))


# TODO: Make this configurable with command line args
suites_to_run = ALL_TESTSUITES[:]


if "--noperf" in sys.argv:
    print("Skipping performance tests")
    suites_to_run.remove("testsuite_perftest")    

for suite in suites_to_run:
    print("\n\n" + "=" * 10, "Running test suite", suite, "=" * 10, "\n")
    getattr(p3d_ecs, suite)()

if not os.path.isfile("test-errors.txt"):
    print("Tests successfully finished!")
    sys.exit(0)

else:
    print("Errors during test!")
    with open("test-errors.txt", "r") as handle:
        for line in handle.readlines():
            print(line)
print("\nTests finished with errors!")
sys.exit(1)

