#include "can_send.h"

int main()
{

    struct sockaddr_can addr;
    struct ifreq ifr;

    // initialization
    if (can_send_init(ifr, addr))
        return 1;

    // Parameter NodeId
    uint8_t NodeId = 127;

    // Motor parameters
    uint16_t EncoderLines = 1024;
    uint16_t EncoderResolution = EncoderLines * 4;

    //  Controller Feedback (encoder OR hall sensors)
    // Encoder Feedback
    Can_Sdo_Write(NodeId, 0x4350, 1, 0x96A);             // Encoder feedback for the velocity controller
    Can_Sdo_Write(NodeId, 0x4550, 1, 0x96A);             // Encoder feedback for the secondary velocity controller
    Can_Sdo_Write(NodeId, 0x4962, 1, EncoderResolution); // Encoder resolution

    // -----------------------------------------------------------------------------------------------------------------------
    //  Controller parameters
    //  SampleTime = 1ms   (min 1ms, max 2ms
    //  Velocity or Position Controller
    Can_Sdo_Write(NodeId, 0x4310, 1, 100);  //  PID Vel Kp (proportional factor)
    Can_Sdo_Write(NodeId, 0x4311, 1, 0);    //  PID Vel Ki (integral factor)
    Can_Sdo_Write(NodeId, 0x4312, 1, 1000); //  PID Vel Kd (differential factor)

    // Can_Sdo_Write(NodeId, 0x4318, 1, -10);                   //  KdSampleTime = RegSampleTime * 10 (+10 = 10*100uS)
    Can_Sdo_Write(NodeId, 0x4313, 1, 10000); //  PID Vel KiLimit (integral limit)
    Can_Sdo_Write(NodeId, 0x4314, 1, 1000);  //  PID Vel Kvff (velocity feed forward)
    
    // //  Secondary Velocity Controller
    Can_Sdo_Write(NodeId, 0x4510, 1, 100); //  PI SVel Kp (proportional factor)
    Can_Sdo_Write(NodeId, 0x4511, 1, 100); //  PI SVel Ki (integral factor)

    //sleep for 5 miliseconds to wait the bus to be empty
    usleep(5*1000);

    Can_Sdo_Write(NodeId, 0x4517, 1, 0);          //  SVel IxR factor
    Can_Sdo_Write(NodeId, 0x4321, 1, 2147483647); //  Max Positive Velocity (RPM)
    Can_Sdo_Write(NodeId, 0x4323, 1, 2147483647); //  Max Negative Velocity (RPM)
    
    // //  Current Controller
    Can_Sdo_Write(NodeId, 0x4210, 1, 100); //  PI Current controller Kp (proportional factor)
    Can_Sdo_Write(NodeId, 0x4211, 1, 100); //  PI Current controller Ki (integral factor)

    // // -----------------------------------------------------------------------------------------------------------------------
    // //  Current Limits
    // Can_Sdo_Write(NodeId, 0x4221, 1, 20000); //  Current limit - max. positive in mA
    // Can_Sdo_Write(NodeId, 0x4223, 1, 20000); //  Current limit - max. negative in mA

    Can_Sdo_Read(127, 0x4310, 1);

    close_can();

    return 0;
}
