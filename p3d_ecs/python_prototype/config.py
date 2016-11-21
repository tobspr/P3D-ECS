
# Amount of server ticks per second
SERVER_UPDATE_RATE = 60

# Amount of physics steps per second on both client and server
UPDATE_RATE = 33

# Send deltas at least every x milliseconds
AUTO_DELTA_RATE = 350

# Controls the maximum amount of deltas sent by the server per second
MAX_DELTAS_PER_FRAME = UPDATE_RATE

# Execute events n milliseconds after being executed on the client
EVENT_DELAY_MS = 100

# Execute deltas n milliseconds after being sent
DELTA_DELAY_MS = 300

# Maximum interpolation duration, after which interpolation is no longer performed
MAX_INTERPOLATION_DELAY = 2.0

# Time it takes to animate the current entity position to its server side position,
# when prediction errors occured
PREDICTION_CORRECTION = 0.07

DELTA_RESEND_RATE = 0.25

# ----- PACKETS -------

SIMULATED_PING_MIN = 25
SIMULATED_PING_MAX = 35
SIMULATED_PACKET_DROP = 0.01


# ------ INTERNAL ------------

EVENT_DELAY = max(1, int(EVENT_DELAY_MS * UPDATE_RATE / 1000.0))
DELTA_DELAY = max(1, int(DELTA_DELAY_MS * UPDATE_RATE / 1000.0))

