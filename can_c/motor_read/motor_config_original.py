#-----------------------------------------------------------------------------------------------------------------------
# Import
from mc.dsa import *

#-----------------------------------------------------------------------------------------------------------------------
# Parameter: NodeId
NodeId = 127                                             # Node Id

#-----------------------------------------------------------------------------------------------------------------------
# Motor parameters
EncoderLines = 1024                                     # BG75's encoder lines
EncoderResolution = 4 * EncoderLines                     # Encoder resolution

#-----------------------------------------------------------------------------------------------------------------------
# Initialisation

#-----------------------------------------------------------------------------------------------------------------------
# Controller Feedback (encoder OR hall sensors)
# Encoder Feedback
mc.Can.SdoWr(NodeId, 0x4350, 1, 0x96A)                   # Encoder feedback for the velocity controller
mc.Can.SdoWr(NodeId, 0x4550, 1, 0x96A)                   # Encoder feedback for the secondary velocity controller
mc.Can.SdoWr(NodeId, 0x4962, 1, EncoderResolution)       # Encoder resolution

#-----------------------------------------------------------------------------------------------------------------------
# Controller parameters
#mc.Can.SdoWr(NodeId, 0x43A0, 1, 1000)                    # SampleTime = 1ms   (min 1ms, max 2ms
# Velocity or Position Controller
mc.Can.SdoWr(NodeId, 0x4310, 1, 100)                     # PID Vel Kp (proportional factor)
mc.Can.SdoWr(NodeId, 0x4311, 1, 0)                       # PID Vel Ki (integral factor)
mc.Can.SdoWr(NodeId, 0x4312, 1, 1000)                      # PID Vel Kd (differential factor)

#mc.Can.SdoWr(NodeId, 0x4318, 1, -10)                    # KdSampleTime = RegSampleTime * 10 (+10 = 10*100uS)
mc.Can.SdoWr(NodeId, 0x4313, 1, 10000)                       # PID Vel KiLimit (integral limit)
mc.Can.SdoWr(NodeId, 0x4314, 1, 1000)                       # PID Vel Kvff (velocity feed forward)

# Secondary Velocity Controller
mc.Can.SdoWr(NodeId, 0x4510, 1, 100)                     # PI SVel Kp (proportional factor)
mc.Can.SdoWr(NodeId, 0x4511, 1, 100)                     # PI SVel Ki (integral factor)

mc.Can.SdoWr(NodeId, 0x4517, 1, 0)                       # SVel IxR factor
mc.Can.SdoWr(NodeId, 0x4321, 1, 2147483647)              # Max Positive Velocity (RPM)
mc.Can.SdoWr(NodeId, 0x4323, 1, 2147483647)              # Max Negative Velocity (RPM)

# Current Controller
mc.Can.SdoWr(NodeId, 0x4210, 1, 100)                     # PI Current controller Kp (proportional factor)
mc.Can.SdoWr(NodeId, 0x4211, 1, 100)                     # PI Current controller Ki (integral factor)

#-----------------------------------------------------------------------------------------------------------------------
# Current Limits
mc.Can.SdoWr(NodeId, 0x4221, 1, 20000)                    # Current limit - max. positive in mA
mc.Can.SdoWr(NodeId, 0x4223, 1, 20000)                    # Current limit - max. negative in mA



########TODO########################




#-----------------------------------------------------------------------------------------------------------------------
# Ramps
# Acceleration = delta V / delta T
mc.Can.SdoWr(NodeId, 0x4340, 1, 5000)           # VEL_Acc_dV (velocity in rpm)

#mc.Can.SdoWr(NodeId, 0x4340, 1, 4294967295)           # VEL_Acc_dV (velocity in rpm)

mc.Can.SdoWr(NodeId, 0x4341, 1, 10)                    # VEL_Acc_dT (time in ms)

# Deceleration = delta V / delta T
mc.Can.SdoWr(NodeId, 0x4342, 1, 5000)                    # Vel_Dec_dV (velocity in rpm)

mc.Can.SdoWr(NodeId, 0x4343, 1, 10)                    # Vel_Dec_dt (time in ms)

#-----------------------------------------------------------------------------------------------------------------------
# Position following error - window
mc.Can.SdoWr(NodeId, 0x4732, 1, 0xFFFFFFFF)                    # Position following error - window

mc.Can.SdoWr(NodeId, 0x473B, 2, 0xFFFF)                   # Position Window Timeout not active


#-----------------------------------------------------------------------------------------------------------------------
# Device mode
mc.Can.SdoWr(NodeId, 0x4003, 1, 7)                       # Device mode "position mode"


##############################################################################################

##### Fisrt TX-PDO #####
TXCobId = 0x180 + NodeId                                 # CobId for the first TX-PDO (motor is sending)
RXCobId = 0x200 + NodeId                                 # CobId for the first RX-PDO (motor is receiving)
Transmissiontype = 255                                   # transmissiontype asynchron (255 = asynchronous)

# Mapping table
mc.Can.SdoWr(NodeId, 0x1A00, 0, 0)                       # delete mapping table first

mc.Can.SdoWr(NodeId, 0x1A00, 1, 0x47620120)              # set first object (actual position):
mc.Can.SdoWr(NodeId, 0x1A00, 2, 0x47610120)              # set second object (commanded position):

mc.Can.SdoWr(NodeId, 0x1A00, 0, 2)                       # write numbers of objects into the table = 2

# Tansmissiontype
mc.Can.SdoWr(NodeId, 0x1800, 2, 255)                     # asynchronous (see above)
mc.Can.SdoWr(NodeId, 0x1800, 3, 200)                     # inhibit time in 100�s



##### 2nd TX-PDO #####
TXCobId = 0x280 + NodeId                               # CobId for the second TX-PDO (motor is sending     2
RXCobId = 0x300 + NodeId                               # CobId for the second RX-PDO (motor is receiving)   2

# Mapping table
mc.Can.SdoWr(NodeId, 0x1A01, 0, 0)                       # delete mapping table first
mc.Can.SdoWr(NodeId, 0x1A01, 1, 0x48300508)              # set 1st object (PWM Control Value -100/+100):
mc.Can.SdoWr(NodeId, 0x1A01, 2, 0x41140110)              # set 2nd object (Temperature in 0.1 oC):
mc.Can.SdoWr(NodeId, 0x1A01, 3, 0x48500120)              # set 3rd object (System Clock in ms):

mc.Can.SdoWr(NodeId, 0x1A01, 0, 3)                       # write numbers of objects into the table = 3


# Tansmissiontype
mc.Can.SdoWr(NodeId, 0x1801, 2, 255)                     # asynchronous (see above)
mc.Can.SdoWr(NodeId, 0x1801, 3, 200)                     # inhibit time in 100�s




##### 3rd TX-PDO #####
TXCobId = 0x380 + NodeId                               # CobId for the second TX-PDO (motor is sending)
RXCobId = 0x400 + NodeId                               # CobId for the second RX-PDO (motor is receiving)

# Mapping table
mc.Can.SdoWr(NodeId, 0x1A02, 0, 0)                       # delete mapping table first
mc.Can.SdoWr(NodeId, 0x1A02, 1, 0x41130120)              # set first object (Motor Current in mA):
mc.Can.SdoWr(NodeId, 0x1A02, 2, 0x42620220)              # set second object (Filtered Current current in mA):
mc.Can.SdoWr(NodeId, 0x1A02, 0, 2)                       # write numbers of objects into the table = 2

# Tansmissiontype
mc.Can.SdoWr(NodeId, 0x1802, 2, 255)                     # asynchronous (see above)
mc.Can.SdoWr(NodeId, 0x1802, 3, 200)                     # inhibit time in 100�s



##### 4th TX-PDO #####
TXCobId = 0x480 + NodeId                               # CobId for the second TX-PDO (motor is sending)
RXCobId = 0x500 + NodeId                               # CobId for the second RX-PDO (motor is receiving)

# Mapping table
mc.Can.SdoWr(NodeId, 0x1A03, 0, 0)                       # delete mapping table first
mc.Can.SdoWr(NodeId, 0x1A03, 1, 0x42600120)              # set 1st object (Current Desired Value in mA):
mc.Can.SdoWr(NodeId, 0x1A03, 2, 0x4A040220)              # set 2nd object (Actual Velocity in RPM):
mc.Can.SdoWr(NodeId, 0x1A03, 0, 2)                       # write numbers of objects into the table = 2

# Tansmissiontype
mc.Can.SdoWr(NodeId, 0x1803, 2, 255)                     # asynchronous (see above)
mc.Can.SdoWr(NodeId, 0x1803, 3, 200)                     # inhibit time in 100�s


#-----------------------------------------------------------------------------------------------------------------------
# Start of the test program
mc.Can.SdoWr(NodeId, 0x4000, 1, 1)                       # Reset error register
mc.Can.SdoWr(NodeId, 0x4004, 1, 1)                       # Enable power stage
mc.Delay(100)                                            # 100ms delay for power stage to be enabled

mc.Can.PdoWr(0, [0x01, NodeId]) # motor in operational mode to star to send PDOs

mc.Can.SdoWr(NodeId, 0x4300, 0x01, 100)               # VEL_DesiredValue [rpm]

mc.Can.SdoWr(NodeId, 0x4791, 1, 4096)                 # Relative positioning 4000 counts (hall or encoder)
mc.Delay(500)

mc.Can.PdoWr(0, [0x80, NodeId]) # motor in pre-operational mode to stop PDOs