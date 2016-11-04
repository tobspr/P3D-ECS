
import panda3d
import panda3d.core

from p3d_ecs.native.p3d_ecs import *

with open("test-output.txt", "w") as handle:
    pass

# input("Press enter to start: ")


testcase_entityref()
testcase_generic()
testcase_parent_child()
testcase_collectors()
testcase_serialization()

perftest_entities()
 
print("Done!")
