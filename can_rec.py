import can
import time

can_interface = 'can0'
bus = can.interface.Bus(can_interface, bustype='socketcan',bitrate=500000)

while True:
	message = bus.recv()
	print(message)
#for msg in bus:
	#print(msg.data)