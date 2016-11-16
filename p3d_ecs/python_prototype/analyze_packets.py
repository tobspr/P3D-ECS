import re

targets = ["server", "client"]

output = []
messages = []

from message import Message

output.append(["timestamp"] + targets)

packet_re = re.compile(r"^([0-9]+): (recv|sent) packet, reliable=(0|1), size=([0-9]+), data=(.*)")

class Packet(object):
    pass

for i, target in enumerate(targets):
    with open("packets-" + target + ".log") as handle:
        for line in handle.readlines():
            line = line.strip()
            if line:
                parts = packet_re.match(line)
                p = Packet()
                p.timestamp = int(parts.group(1))
                p.sent_or_recv = parts.group(2)
                p.reliable = bool(int(parts.group(3)))
                p.size = int(parts.group(4))
                p.data = parts.group(5)
                p.index = i
                messages.append(p)

messages = list(sorted(messages, key=lambda packet: packet.timestamp))
first_timestamp = messages[0].timestamp


for message in messages:
    parts = message.data.split(Message.DIVIDER)

    serialized = message.sent_or_recv.upper() + " [" + parts[0] + "] " + parts[1]
    line = [""] * (len(targets) + 1)
    print(message.timestamp)
    line[0] = str( int((message.timestamp - first_timestamp) / (1000.0 ** 2) )) + " ms"
    line[message.index + 1] = serialized
    output.append(line)

with open("packets.csv", "w") as handle:
    for line in output:
        handle.write(";".join(line) + "\n")


