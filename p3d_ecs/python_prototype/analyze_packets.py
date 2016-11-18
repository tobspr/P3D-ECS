import re
import os

targets = []

for f in os.listdir("."):
    if f.startswith("packets-") and f.endswith(".log"):
        name = f[8:-4]
        targets.append(name)

print("Targets:", *targets)

output = []
messages = []

output.append(["timestamp"] + targets)

log_re = re.compile(r"^([0-9]+) \[([0-9 ]+)\] (.*)")

class LogEntry(object):
    pass

for i, target in enumerate(targets):
    with open("packets-" + target + ".log") as handle:
        for line in handle.readlines():
            line = line.strip()
            if line:
                parts = log_re.match(line)

                p = LogEntry()
                p.timestamp = int(parts.group(1))
                p.sim_time = int(parts.group(2))
                p.content = parts.group(3)
                p.index = i
                messages.append(p)

messages = list(sorted(messages, key=lambda packet: packet.timestamp))
first_timestamp = messages[0].timestamp

for message in messages:
    serialized = "[" + str(message.sim_time).ljust(6) + "] " + message.content
    line = [""] * (len(targets) + 1)
    line[0] = str(int((message.timestamp - first_timestamp) / 1000000.0)) + " ms"
    line[message.index + 1] = serialized
    output.append(line)

with open("packets.csv", "w") as handle:
    for line in output:
        handle.write(";".join(line) + "\n")


