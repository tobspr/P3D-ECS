#!/usr/bin/env python
# Super simple script that listens to a local UDP port and relays all packets to an arbitrary remote host.
# Packets that the host sends back will also be relayed to the local UDP client.
# Works with Python 2 and 3

import sys, socket
import time
import random
import config

def fail(reason):
    sys.stderr.write(reason + '\n')
    sys.exit(1)

# if len(sys.argv) != 2 or len(sys.argv[1].split(':')) != 3:
#     fail('Usage: udp-relay.py localPort:remoteHost:remotePort')

# localPort, remoteHost, remotePort = sys.argv[1].split(':')
localPort, remoteHost, remotePort = 5553, "127.0.0.1", 5555

try:
    localPort = int(localPort)
except:
    fail('Invalid port number: ' + str(localPort))
try:
    remotePort = int(remotePort)
except:
    fail('Invalid port number: ' + str(remotePort))

try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind(('', localPort))
except:
    fail('Failed to bind on port ' + str(localPort))

knownClient = None
knownServer = (remoteHost, remotePort)
sys.stderr.write('All set.\n')

s.setblocking(0)

packets_to_send = []

first_sent = False

incoming = 0
outgoing = 0
loss = 0
sent = 0
last_stats = time.time()

def get_delayed_time():
    t = time.time()
    t += random.randint(config.SIMULATED_PING_MIN, config.SIMULATED_PING_MAX) / 1000.0
    return t

while True:
    data = None
    try:
        data, addr = s.recvfrom(32768)
    except:
        pass

    if data:
        if knownClient is None:
            knownClient = addr
        if addr == knownClient:
            packets_to_send.append((get_delayed_time(), data, knownServer))
        else:
            packets_to_send.append((get_delayed_time(), data, knownClient))

    did_sth = True
    while packets_to_send and did_sth:
        did_sth = False
        t, data, addr = packets_to_send[0]
        if t <= time.time():
            t, data, addr = packets_to_send.pop(0)
            did_sth = True
            if (random.random() < config.SIMULATED_PACKET_DROP) and first_sent:
                # print("Dropping ", len(data))
                loss += 1
                pass
            else:
                # print("Forward", len(data), "to", *addr)
                s.sendto(data, addr)
                first_sent = True
                sent += 1

                if addr == knownServer:
                    incoming += len(data)
                else:
                    outgoing += len(data)

    if time.time() - last_stats > 1.0:
        last_stats = time.time()
        print("{:3.2f} KB/s incoming, {:3.2f} KB/s outgoing, {} sent, {} lost".format(incoming / 1024.0, outgoing / 1024.0, sent, loss))
        incoming = 0
        outgoing = 0 
        loss = 0
        sent = 0

    time.sleep(2.0 / 1000.0)