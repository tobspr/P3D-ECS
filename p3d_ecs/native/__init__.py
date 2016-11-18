
import os
import sys
print("Loading protobuf ..")
protobuf_loc = os.path.join(os.path.dirname(__file__), "..", "..", "thirdparty", "protobuf-built")
print("AT:", protobuf_loc)
assert os.path.isdir(protobuf_loc)
sys.path.insert(0, protobuf_loc)
