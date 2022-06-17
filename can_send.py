import time
import can

bustype = 'socketcan'
channel = 'can0'
bus = can.interface.Bus(channel=channel, bustype=bustype,bitrate=500000)

msg = can.Message(arbitration_id=0xc0ffee, data=[0, 1, 3, 1, 4, 1], is_extended_id=False)
while True:
	bus.send(msg)
	time.sleep(1)