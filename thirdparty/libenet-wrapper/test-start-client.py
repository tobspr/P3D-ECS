
import time
import _thread
import panda3d.core 
import libenetwrap

socket = libenetwrap.ENetClientSocketPy()
print("Starting socket ..")
peer = socket.connect("localhost", 12345)
if peer:
    print("Running connection")
    print("Sending hello packet")
    peer.send_message("Hello world!")
    
    print("Checking for messages")
    while True:
        event = socket.poll()
        if event.type == libenetwrap.ENetSocketEvent.Disconnect:
            print("SYS: Connection closed")
            break

        while peer.has_messages:
            message = peer.take_message()
            print("RECIEVED MESSAGE: '" + message + "'")

print("Done!")
