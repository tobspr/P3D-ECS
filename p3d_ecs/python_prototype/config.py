
# Amount of server ticks per second
SERVER_UPDATE_RATE = 60

# Amount of physics steps per second on both client and server
UPDATE_RATE = 27

# Send deltas at least every x milliseconds
AUTO_DELTA_RATE = 350000000

# Controls the maximum amount of deltas sent by the server per second
MAX_DELTAS_PER_FRAME = UPDATE_RATE

# Execute events n milliseconds after being executed on the client
EVENT_DELAY_MS = 50

# Execute deltas n milliseconds after being sent
DELTA_DELAY_MS = 400

# Maximum interpolation duration, after which interpolation is no longer performed
MAX_INTERPOLATION_DELAY = 2.0

# Time it takes to animate the current entity position to its server side position,
# when prediction errors occured
PREDICTION_CORRECTION = 1.0


# Drop n% of snapshots to simulate bad internet
SIMULATED_SNAPSHOT_DROP_RATE = 0.8


# ------ INTERNAL ------------

EVENT_DELAY = max(1, int(EVENT_DELAY_MS * UPDATE_RATE / 1000.0))
DELTA_DELAY = max(1, int(DELTA_DELAY_MS * UPDATE_RATE / 1000.0))

