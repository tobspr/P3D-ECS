
import panda3d
import panda3d.core

from p3d_ecs.native.p3d_ecs import *

with open("test-output.txt", "w") as handle:
    pass

# input("Press enter to start: ")

testcase_parent_child()
testcase_generic()
testcase_collectors()

#perftest_entities()
 
print("Done!")
