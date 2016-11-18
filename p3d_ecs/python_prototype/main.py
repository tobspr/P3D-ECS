"""

Simple entity system prototype for testing new features

"""

import os
import sys
import time
import random
from _thread import start_new_thread

from panda3d.core import *

from game_client import GameClient
from server import ServerApp

from libenet import get_time

if __name__ == "__main__":
    IP = "localhost"
    PORT = random.randint(1025, 2**16 - 1)
    PORT = 5553

    print("TIME", get_time())

    def client_main():
        print("Initializing client ..")
        load_prc_file_data("", """
            sync-video #f
            win-size 600 400
            window-title CLIENT
            show-frame-rate-meter #t
        """)
        client = GameClient(IP, PORT)
        client.run()

        print("Done.")

    def server_main():
        print("Starting local server ..")
        load_prc_file_data("", """
            sync-video #f
            win-size 600 400
            window-title SERVER
            show-frame-rate-meter #t
        """)
        server = ServerApp(IP, PORT)
        server.run()

    if "--client" in sys.argv:
        time.sleep(0.3)
        client_main()
    elif "--server" in sys.argv:
        time.sleep(0.1)
        server_main()
    else:
        # parallel
        start_new_thread(server_main, ())
        time.sleep(0.5)
        client_main()
    os._exit(0)

