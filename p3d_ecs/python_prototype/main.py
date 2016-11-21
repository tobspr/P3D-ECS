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

from libenet import fast_time

import builtins

if __name__ == "__main__":
    IP = "localhost"
    PORT = random.randint(1025, 2**16 - 1)

    print("TIME", fast_time())

    def client_main():
        __builtins__.IS_SERVER = False
        PORT = 5553

        print("Initializing client ..")
        win_size = "600 400"
        win_origin = "100 550"
        if "--number=2" in sys.argv:
            win_size = "1050 900"
            win_origin = "800 50"

        load_prc_file_data("", """
            sync-video #t
            win-size """ + win_size + """
            win-origin """ + win_origin + """
            window-title CLIENT
            show-frame-rate-meter #t
        """)
        client = GameClient(IP, PORT)
        client.run()

        print("Done.")

    def server_main():
        __builtins__.IS_SERVER = True
        PORT = 5555


        print("Starting local server ..")
        load_prc_file_data("", """
            sync-video #f
            win-size 600 400
            win-origin 100 50
            window-title SERVER
            show-frame-rate-meter #t
        """)
        server = ServerApp(IP, PORT)
        server.run()


    if "--client" in sys.argv:
        time.sleep(0.5)
        client_main()
    elif "--server" in sys.argv:
        time.sleep(0.2)
        server_main()
    else:
        # parallel
        start_new_thread(server_main, ())
        time.sleep(0.5)
        client_main()
    os._exit(0)

