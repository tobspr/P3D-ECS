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

try:
    os.remove("test-errors.txt")
except:
    pass

ALL_TESTSUITES = [i for i, _ in inspect.getmembers(
    p3d_ecs) if i.startswith("testsuite_")]

# TODO: Make this configurable with command line args
suites_to_run = ALL_TESTSUITES[:]


# suites_to_run.remove("testsuite_perftest")
# For local testing
# suites_to_run = ["testsuite_serialization"]
suites_to_run = ["testsuite_perftest"]

# Run all tests in CI
if "--ci" in sys.argv:
    print("Running CI Build ..")
    suites_to_run = ALL_TESTSUITES[:]

for suite in suites_to_run:
    print("\n\n" + "=" * 10, "Running test suite", suite, "=" * 10, "\n")
    getattr(p3d_ecs, suite)()

stats = p3d_ecs.UnittestStats
print("\n\n")
print("=" * 80)
print("\n")
print("STATISTICS:")
print("Testcases:", stats.get_num_testcases_ran())
print("Uncatched Warnings:", stats.get_num_uncatched_warnings())
print("Passed expections:", stats.get_num_requires_passed())
print("Failed expections:", stats.get_num_errors_occured())
print("Memory leaks:", stats.get_num_leaks())
print()


if (stats.get_num_errors_occured() == 0 and
    stats.get_num_leaks() == 0 and
        stats.get_num_uncatched_warnings() == 0):
    print("Tests finished SUCCESSFULLY!\n")
    print("=" * 80)
    sys.exit(0)

else:
    print("Tests FAILED!\n")
    try:
        with open("test-errors.txt", "r") as handle:
            for line in handle.readlines():
                if line.strip():
                    print(line.strip())
    except Exception:
        pass
    print("\n!! Tests finished with", stats.get_num_errors_occured(), "errors,",
          stats.get_num_uncatched_warnings(), "uncatched warnings and",
          stats.get_num_leaks(), "leaks !!\n")
    print("=" * 80)
    sys.exit(1)
