#include "can_lib.h"

int main()
{

    struct sockaddr_can addr;
    struct ifreq ifr;

    // initialization
    if (can_send_init(ifr, addr))
        return 1;
    if(create_log_file())
        return 1;

    // Parameter NodeId
    uint8_t NodeId = 127;
    uint16_t TXCobId, RXCobId;

    // Motor parameters
    uint16_t EncoderLines = 1024;
    uint16_t EncoderResolution = EncoderLines * 4;

    // test for some error messages
    // Can_Sdo_Write(NodeId, 0x4111, 1, 10);             // Encoder feedback for the velocity controller
    // Can_Sdo_read_and_check();

    //  Controller Feedback (encoder OR hall sensors)
    // Encoder Feedback
    Can_Sdo_Write(NodeId, 0x4350, 1, 0x96A); // Encoder feedback for the velocity controller
    /* the correct response should be ID: 5ff DLC: 8 Dta: 60 50 43 01 00 00 00 00 */
    Can_Sdo_read_and_check(NodeId, 0x4350, 1);
    Can_Sdo_Write(NodeId, 0x4550, 1, 0x96A); // Encoder feedback for the secondary velocity controller
    Can_Sdo_read_and_check(NodeId, 0x4550, 1);
    Can_Sdo_Write(NodeId, 0x4962, 1, EncoderResolution); // Encoder resolution
    Can_Sdo_read_and_check(NodeId, 0x4962, 1);

    // -----------------------------------------------------------------------------------------------------------------------
    //  Controller parameters
    //  SampleTime = 1ms   (min 1ms, max 2ms
    //  Velocity or Position Controller
    Can_Sdo_Write(NodeId, 0x4310, 1, 100); //  PID Vel Kp (proportional factor)
    Can_Sdo_read_and_check(NodeId, 0x4310, 1);
    Can_Sdo_Write(NodeId, 0x4311, 1, 0); //  PID Vel Ki (integral factor)
    Can_Sdo_read_and_check(NodeId, 0x4311, 1);
    Can_Sdo_Write(NodeId, 0x4312, 1, 1000); //  PID Vel Kd (differential factor)
    Can_Sdo_read_and_check(NodeId, 0x4312, 1);

    // Can_Sdo_Write(NodeId, 0x4318, 1, -10);                   //  KdSampleTime = RegSampleTime * 10 (+10 = 10*100uS)
    Can_Sdo_Write(NodeId, 0x4313, 1, 10000); //  PID Vel KiLimit (integral limit)
    Can_Sdo_read_and_check(NodeId, 0x4313, 1);
    Can_Sdo_Write(NodeId, 0x4314, 1, 1000); //  PID Vel Kvff (velocity feed forward)
    Can_Sdo_read_and_check(NodeId, 0x4314, 1);

    // //  Secondary Velocity Controller
    Can_Sdo_Write(NodeId, 0x4510, 1, 100); //  PI SVel Kp (proportional factor)
    Can_Sdo_read_and_check(NodeId, 0x4510, 1);
    Can_Sdo_Write(NodeId, 0x4511, 1, 100); //  PI SVel Ki (integral factor)
    Can_Sdo_read_and_check(NodeId, 0x4511, 1);

    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);

    Can_Sdo_Write(NodeId, 0x4517, 1, 0); //  SVel IxR factor
    Can_Sdo_read_and_check(NodeId, 0x4517, 1);
    Can_Sdo_Write(NodeId, 0x4321, 1, 2147483647); //  Max Positive Velocity (RPM)
    Can_Sdo_read_and_check(NodeId, 0x4321, 1);
    Can_Sdo_Write(NodeId, 0x4323, 1, 2147483647); //  Max Negative Velocity (RPM)
    Can_Sdo_read_and_check(NodeId, 0x4323, 1);

    // //  Current Controller
    Can_Sdo_Write(NodeId, 0x4210, 1, 100); //  PI Current controller Kp (proportional factor)
    Can_Sdo_read_and_check(NodeId, 0x4210, 1);
    Can_Sdo_Write(NodeId, 0x4211, 1, 100); //  PI Current controller Ki (integral factor)
    Can_Sdo_read_and_check(NodeId, 0x4211, 1);

    // -----------------------------------------------------------------------------------------------------------------------
    //  Current Limits
    Can_Sdo_Write(NodeId, 0x4221, 1, 20000); //  Current limit - max. positive in mA
    Can_Sdo_read_and_check(NodeId, 0x4221, 1);
    Can_Sdo_Write(NodeId, 0x4223, 1, 20000); //  Current limit - max. negative in mA
    Can_Sdo_read_and_check(NodeId, 0x4223, 1);

    // // -----------------------------------------------------------------------------------------------------------------------
    // // Ramps
    // Acceleration = delta V / delta T
    Can_Sdo_Write(NodeId, 0x4340, 1, 5000); // VEL_Acc_dV (velocity in rpm)
    Can_Sdo_read_and_check(NodeId, 0x4340, 1);

    // Can_Sdo_Write(NodeId, 0x4340, 1, 4294967295)         // VEL_Acc_dV (velocity in rpm)

    Can_Sdo_Write(NodeId, 0x4341, 1, 10); // VEL_Acc_dT (time in ms)
    Can_Sdo_read_and_check(NodeId, 0x4341, 1);

    // // Deceleration = delta V / delta T
    Can_Sdo_Write(NodeId, 0x4342, 1, 5000); // Vel_Dec_dV (velocity in rpm)
    Can_Sdo_read_and_check(NodeId, 0x4342, 1);
    Can_Sdo_Write(NodeId, 0x4343, 1, 10); // Vel_Dec_dt (time in ms)
    Can_Sdo_read_and_check(NodeId, 0x4343, 1);
    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);
    // -----------------------------------------------------------------------------------------------------------------------
    // Position following error - window
    Can_Sdo_Write(NodeId, 0x4732, 1, 0xFFFFFFFF); // Position following error - window
    Can_Sdo_read_and_check(NodeId, 0x4732, 1);

    Can_Sdo_Write(NodeId, 0x473B, 2, 0xFFFF); // Position Window Timeout not active
    Can_Sdo_read_and_check(NodeId, 0x473B, 2);

    //-----------------------------------------------------------------------------------------------------------------------
    // Device mode
    Can_Sdo_Write(NodeId, 0x4003, 1, 7); // Device mode "position mode"
    Can_Sdo_read_and_check(NodeId, 0x4003, 1);

    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);

    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * @todo
     */

    // ////////// Fisrt TX-PDO //////////
    TXCobId = 0x180 + NodeId;       // CobId for the first TX-PDO (motor is sending)
    RXCobId = 0x200 + NodeId;       // CobId for the first RX-PDO (motor is receiving)
    uint8_t Transmissiontype = 255; // transmissiontype asynchron (255 = asynchronous)

    // // Mapping table
    Can_Sdo_Write(NodeId, 0x1A00, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A00, 0);

    Can_Sdo_Write(NodeId, 0x1A00, 1, 0x47620120); // set first object (actual position):
    Can_Sdo_read_and_check(NodeId, 0x1A00, 1);
    Can_Sdo_Write(NodeId, 0x1A00, 2, 0x47610120); // set second object (commanded position):
    Can_Sdo_read_and_check(NodeId, 0x1A00, 2);

    Can_Sdo_Write(NodeId, 0x1A00, 0, 2); // write numbers of objects into the table = 2
    Can_Sdo_read_and_check(NodeId, 0x1A00, 0);

    // // Tansmissiontype
    Can_Sdo_Write(NodeId, 0x1800, 2, Transmissiontype); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1800, 2);
    Can_Sdo_Write(NodeId, 0x1800, 3, 200); // inhibit time in 100
    Can_Sdo_read_and_check(NodeId, 0x1800, 3);
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000

    // ////////// 2nd TX-PDO //////////
    TXCobId = 0x280 + NodeId; // CobId for the second TX-PDO (motor is sending     2
    RXCobId = 0x300 + NodeId; // CobId for the second RX-PDO (motor is receiving)   2

    // Mapping table
    Can_Sdo_Write(NodeId, 0x1A01, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A01, 0);
    Can_Sdo_Write(NodeId, 0x1A01, 1, 0x48300508); // set 1st object (PWM Control Value -100/+100):
    Can_Sdo_read_and_check(NodeId, 0x1A01, 1);
    Can_Sdo_Write(NodeId, 0x1A01, 2, 0x41140110); // set 2nd object (Temperature in 0.1 oC):
    Can_Sdo_read_and_check(NodeId, 0x1A01, 2);
    Can_Sdo_Write(NodeId, 0x1A01, 3, 0x48500120); // set 3rd object (System Clock in ms):
    Can_Sdo_read_and_check(NodeId, 0x1A01, 3);

    Can_Sdo_Write(NodeId, 0x1A01, 0, 3); // write numbers of objects into the table = 3
    Can_Sdo_read_and_check(NodeId, 0x1A01, 0);

    // Tansmissiontype
    Can_Sdo_Write(NodeId, 0x1801, 2, Transmissiontype); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1801, 2);
    Can_Sdo_Write(NodeId, 0x1801, 3, 200); // inhibit time in 100
    Can_Sdo_read_and_check(NodeId, 0x1801, 3);

    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);
    ////////// 3rd TX-PDO //////////
    TXCobId = 0x380 + NodeId; // CobId for the second TX-PDO (motor is sending)
    RXCobId = 0x400 + NodeId; // CobId for the second RX-PDO (motor is receiving)

    // Mapping table
    Can_Sdo_Write(NodeId, 0x1A02, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A02, 0);
    Can_Sdo_Write(NodeId, 0x1A02, 1, 0x41130120); // set first object (Motor Current in mA):
    Can_Sdo_read_and_check(NodeId, 0x1A02, 1);

    Can_Sdo_Write(NodeId, 0x1A02, 2, 0x42620220); // set second object (Filtered Current current in mA):
    Can_Sdo_read_and_check(NodeId, 0x1A02, 2);
    Can_Sdo_Write(NodeId, 0x1A02, 0, 2); // write numbers of objects into the table = 2
    Can_Sdo_read_and_check(NodeId, 0x1A02, 0);
    // Tansmissiontype
    Can_Sdo_Write(NodeId, 0x1802, 2, Transmissiontype); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1802, 2);
    Can_Sdo_Write(NodeId, 0x1802, 3, 200); // inhibit time in 100
    Can_Sdo_read_and_check(NodeId, 0x1802, 3);

    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);

    ////////// 4th TX-PDO //////////
    TXCobId = 0x480 + NodeId; // CobId for the second TX-PDO (motor is sending)
    RXCobId = 0x500 + NodeId; // CobId for the second RX-PDO (motor is receiving)

    // Mapping PDO table
    Can_Sdo_Write(NodeId, 0x1A03, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A03, 0);
    Can_Sdo_Write(NodeId, 0x1A03, 1, 0x42600120); // set 1st object (Current Desired Value in mA):
    Can_Sdo_read_and_check(NodeId, 0x1A03, 1);
    Can_Sdo_Write(NodeId, 0x1A03, 2, 0x4A040220); // set 2nd object (Actual Velocity in RPM):
    Can_Sdo_read_and_check(NodeId, 0x1A03, 2);
    Can_Sdo_Write(NodeId, 0x1A03, 0, 2); // write numbers of objects into the table = 2
    Can_Sdo_read_and_check(NodeId, 0x1A03, 0);

    // Tansmissiontype
    Can_Sdo_Write(NodeId, 0x1803, 2, Transmissiontype); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1803, 2);
    Can_Sdo_Write(NodeId, 0x1803, 3, 200); // inhibit time in 100
    Can_Sdo_read_and_check(NodeId, 0x1803, 3);

    //-----------------------------------------------------------------------------------------------------------------------
    /**
     * @brief Start of the test program
     *
    //  */
    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);

    Can_Sdo_Write(NodeId, 0x4000, 1, 1); // Reset error register
    Can_Sdo_read_and_check(NodeId, 0x4000, 1);
    Can_Sdo_Write(NodeId, 0x4004, 1, 1); // Enable power stage
    Can_Sdo_read_and_check(NodeId, 0x4004, 1);
    // usleep(100 * 1000);                        // 100ms delay for power stage to be enabled

    // data=[0x01, NodeId,0x00,0x00...]
    // mc.Can.PdoWr(0, data) # motor in operational mode to star to send PDOs

    // mc.Can.SendTwo(0, 0x01, NodeId) # motor in operational mode to star to send PDOs

    Can_Sdo_Write(NodeId, 0x4300, 0x01, 100); // VEL_DesiredValue [rpm]
    Can_Sdo_read_and_check(NodeId, 0x4300, 0x01);

    Can_Sdo_Write(NodeId, 0x4791, 1, 4096); // Relative positioning 4000 counts (hall or encoder)
    Can_Sdo_read_and_check(NodeId, 0x4791, 1);

    /*start the PDO*/
    uint8_t pdo_data_1[2] = {0x01, NodeId};
    Can_Pdo_Write(0, pdo_data_1, sizeof(pdo_data_1) / sizeof(pdo_data_1[0]));





    for (int i = 0; i < 5; i++)
    {
        // PDO Logging thread starts
        start_Pdo_logging();
        /*wait for some time before we write some SDO messages*/

        /**
         * @brief The uspleep time can not be exactly 20 ms, since the motor period is 20 ms
         *        otherwise this wil collides with the motor own frequency (CAN message collision)
         * 
         */
        usleep(15000);
        // 2022-07-18
        // Kill the PDO thread and then send the SDO message (Can_Sdo_Write_while_Pdo_logging)
        // Can_Sdo_read_and_check_while_Pdo_logging
        // If a SDO response of motor is sent,
        // break the loop

        Can_Sdo_write_while_Pdo_logging(NodeId, 0x4004, 1, 1);
        Can_Sdo_read_and_check_while_Pdo_logging(NodeId, 0x4004, 1);

        // PDO Logging thread starts
    }




    uint8_t pdo_data_2[2] = {0x80, NodeId};
    Can_Pdo_Write(0, pdo_data_2, sizeof(pdo_data_2) / sizeof(pdo_data_2[0]));

    Can_Sdo_Write(NodeId, 0x4004, 1, 0); // Disable power stage
    /**
     * @brief uncomment this later
     *
     */
    // Can_Sdo_read_and_check(NodeId, 0x4004, 1);

    /**
     * @brief clean up
     */
    printf("Program run time: %f  \r\n", (double)clock() / (CLOCKS_PER_SEC));

    printf("Demo finished!\n");

    close_can();

    return 0;
}
