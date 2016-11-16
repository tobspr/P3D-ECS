import time
import sys
import panda3d.core 
import libenetwrap

socket = libenetwrap.ENetServerSocketPy()
print("Starting socket ..")
if socket.start(port=12345, max_connections=32, max_channels=2):

    while True:
        event = socket.poll()
        if event.type == event.Disconnect:
            print("DISCONNECTED!")
            sys.exit(-1)
        for i in range(socket.num_peers):
            peer = socket.get_peer(i)
            while peer.has_messages:
                msg = peer.take_message()
                print(" --> Peer: ", peer, "- Message: '" + msg + "'")
                peer.send_message("OK")
                print("SENDING 'OK'")
        
    print("Done!")
