#include "can_lib.h"

int main()
{

    struct sockaddr_can addr;
    struct ifreq ifr;

    // initialization
    if (can_send_init(ifr, addr))
        return 1;
    if (create_log_file())
        return 1;

    // Parameter NodeId
    uint8_t NodeId = 127;
    uint16_t TXCobId, RXCobId;

    uint8_t pdo_data_2[2] = {0x80, NodeId};
    Can_Pdo_Write(0, pdo_data_2, sizeof(pdo_data_2) / sizeof(pdo_data_2[0]));

    Can_Sdo_Write(NodeId, 0x3004, 0x00, 0); // DEV_Enable - Disable
    Can_Sdo_read_and_check(NodeId, 0x3004, 0x00);

    Can_Sdo_Write(NodeId, 0x3000, 0x00, 1); // DEV_Cmd - Clear error
    Can_Sdo_read_and_check(NodeId, 0x3000, 0x00);

    Can_Sdo_Write(NodeId, 0x3000, 0x00, 0x82); // DEV_Cmd - Default parameter
    Can_Sdo_read_and_check(NodeId, 0x3000, 0x00);

    // usleep(1000000);

    Can_Sdo_Write(NodeId, 0x3900, 0x00, 1); // MOTOR_Type
    Can_Sdo_read_and_check(NodeId, 0x3900, 0x00);

    Can_Sdo_Write(NodeId, 0x3911, 0x00, 0); // MOTOR_Polarity0
    Can_Sdo_read_and_check(NodeId, 0x3911, 0x00);

    Can_Sdo_Write(NodeId, 0x3910, 0x00, 22); // MOTOR_PolN
    Can_Sdo_read_and_check(NodeId, 0x3910, 0x00);
    Can_Sdo_Write(NodeId, 0x3962, 0x00, 2000); // MOTOR_ENC_Resolution
    Can_Sdo_read_and_check(NodeId, 0x3962, 0x00);

    Can_Sdo_Write(NodeId, 0x3901, 0x00, 1770); // MOTOR_Nn
    Can_Sdo_read_and_check(NodeId, 0x3901, 0x00);
    Can_Sdo_Write(NodeId, 0x3902, 0x00, 18000); // MOTOR_Un
    Can_Sdo_read_and_check(NodeId, 0x3902, 0x00);

    Can_Sdo_Write(NodeId, 0x3350, 0x00, 2378); // VEL_Feedback
    Can_Sdo_read_and_check(NodeId, 0x3350, 0x00);
    Can_Sdo_Write(NodeId, 0x3550, 0x00, 2378); // SVEL_Feedback
    Can_Sdo_read_and_check(NodeId, 0x3550, 0x00);

    Can_Sdo_Write(NodeId, 0x3221, 0x00, 30000); // CURR_LimitMaxPos
    Can_Sdo_read_and_check(NodeId, 0x3221, 0x00);
    Can_Sdo_Write(NodeId, 0x3223, 0x00, 30000); // CURR_LimitMaxNeg
    Can_Sdo_read_and_check(NodeId, 0x3223, 0x00);

    Can_Sdo_Write(NodeId, 0x3050, 0x00, 0); // DEV_DinEnable
    Can_Sdo_read_and_check(NodeId, 0x3050, 0x00);
    Can_Sdo_Write(NodeId, 0x3051, 0x00, 0); // DEV_DinClearError
    Can_Sdo_read_and_check(NodeId, 0x3051, 0x00);
    Can_Sdo_Write(NodeId, 0x3052, 0x00, 0); // DEV_DinStartStop
    Can_Sdo_read_and_check(NodeId, 0x3052, 0x00);
    Can_Sdo_Write(NodeId, 0x3055, 0x00, 0); // DEV_DinLimitPos
    Can_Sdo_read_and_check(NodeId, 0x3055, 0x00);
    Can_Sdo_Write(NodeId, 0x3056, 0x00, 0); // DEV_DinLimitNeg
    Can_Sdo_read_and_check(NodeId, 0x3056, 0x00);
    Can_Sdo_Write(NodeId, 0x3057, 0x00, 0); // DEV_DinReference
    Can_Sdo_read_and_check(NodeId, 0x3057, 0x00);

    Can_Sdo_Write(NodeId, 0x3060, 0x00, 0); // DEV_DoutError
    Can_Sdo_read_and_check(NodeId, 0x3060, 0x00);

    Can_Sdo_Write(NodeId, 0x3108, 0x00, 0); // IO_AIN0_Offset
    Can_Sdo_read_and_check(NodeId, 0x3108, 0x00);
    Can_Sdo_Write(NodeId, 0x3108, 0x01, 0); // IO_AIN1_Offset
    Can_Sdo_read_and_check(NodeId, 0x3108, 0x01);
    Can_Sdo_Write(NodeId, 0x310a, 0x00, 0); // IO_AIN0_DeadBand
    Can_Sdo_read_and_check(NodeId, 0x310a, 0x00);
    Can_Sdo_Write(NodeId, 0x310a, 0x01, 0); // IO_AIN1_DeadBand
    Can_Sdo_read_and_check(NodeId, 0x310a, 0x01);

    Can_Sdo_Write(NodeId, 0x3154, 0x00, 255); // DEV_DoutEnable
    Can_Sdo_read_and_check(NodeId, 0x3154, 0x00);

    Can_Sdo_Write(NodeId, 0x3204, 0x00, 512); // CURR_DesiredValue_Source
    Can_Sdo_read_and_check(NodeId, 0x3204, 0x00);
    Can_Sdo_Write(NodeId, 0x3205, 0x00, 1000); // CURR_DesiredValue_Reference
    Can_Sdo_read_and_check(NodeId, 0x3205, 0x00);

    Can_Sdo_Write(NodeId, 0x3210, 0x00, 20); // CURR_Kp
    Can_Sdo_read_and_check(NodeId, 0x3210, 0x00);
    Can_Sdo_Write(NodeId, 0x3211, 0x00, 20); // CURR_Ki
    Can_Sdo_read_and_check(NodeId, 0x3211, 0x00);

    Can_Sdo_Write(NodeId, 0x3224, 0x00, 0); // CURR_DynLimitMode
    Can_Sdo_read_and_check(NodeId, 0x3224, 0x00);
    Can_Sdo_Write(NodeId, 0x3224, 0x01, 30000); // CURR_DynLimitPeak
    Can_Sdo_read_and_check(NodeId, 0x3224, 0x01);
    Can_Sdo_Write(NodeId, 0x3224, 0x02, 30000); // CURR_DynLimitCont
    Can_Sdo_read_and_check(NodeId, 0x3224, 0x02);
    Can_Sdo_Write(NodeId, 0x3224, 0x03, 1000); // CURR_DynLimitTime
    Can_Sdo_read_and_check(NodeId, 0x3224, 0x03);

    Can_Sdo_Write(NodeId, 0x3240, 0x00, 10000); // CURR_Acc_dI
    Can_Sdo_read_and_check(NodeId, 0x3240, 0x00);
    Can_Sdo_Write(NodeId, 0x3241, 0x00, 1); // CURR_Acc_dT
    Can_Sdo_read_and_check(NodeId, 0x3241, 0x00);
    Can_Sdo_Write(NodeId, 0x3242, 0x00, 10000); // CURR_Dec_dI
    Can_Sdo_read_and_check(NodeId, 0x3242, 0x00);
    Can_Sdo_Write(NodeId, 0x3243, 0x00, 1); // CURR_Dec_dT
    Can_Sdo_read_and_check(NodeId, 0x3243, 0x00);
    Can_Sdo_Write(NodeId, 0x3244, 0x00, 10000); // CURR_Dec_QuickStop_dI
    Can_Sdo_read_and_check(NodeId, 0x3244, 0x00);
    Can_Sdo_Write(NodeId, 0x3245, 0x00, 1); // CURR_Dec_QuickStop_dT
    Can_Sdo_read_and_check(NodeId, 0x3245, 0x00);
    Can_Sdo_Write(NodeId, 0x324c, 0x00, 0); // CURR_RampType
    Can_Sdo_read_and_check(NodeId, 0x324c, 0x00);

    Can_Sdo_Write(NodeId, 0x3302, 0x00, 1); // VEL_ScaleNum
    Can_Sdo_read_and_check(NodeId, 0x3302, 0x00);
    Can_Sdo_Write(NodeId, 0x3303, 0x00, 1); // VEL_ScaleDen
    Can_Sdo_read_and_check(NodeId, 0x3303, 0x00);
    Can_Sdo_Write(NodeId, 0x3304, 0x00, 768); // VEL_DesiredValue_Source
    Can_Sdo_read_and_check(NodeId, 0x3304, 0x00);
    Can_Sdo_Write(NodeId, 0x3305, 0x00, 1000); // VEL_DesiredValue_Reference
    Can_Sdo_read_and_check(NodeId, 0x3305, 0x00);
    Can_Sdo_Write(NodeId, 0x330a, 0x00, 1); // VEL_DimensionNum
    Can_Sdo_read_and_check(NodeId, 0x330a, 0x00);
    Can_Sdo_Write(NodeId, 0x330b, 0x00, 1); // VEL_DimensionDen
    Can_Sdo_read_and_check(NodeId, 0x330b, 0x00);

    Can_Sdo_Write(NodeId, 0x3310, 0x00, 100); // VEL_Kp
    Can_Sdo_read_and_check(NodeId, 0x3310, 0x00);
    Can_Sdo_Write(NodeId, 0x3311, 0x00, 30); // VEL_Ki
    Can_Sdo_read_and_check(NodeId, 0x3311, 0x00);
    Can_Sdo_Write(NodeId, 0x3312, 0x00, 0); // VEL_Kd
    Can_Sdo_read_and_check(NodeId, 0x3312, 0x00);
    Can_Sdo_Write(NodeId, 0x3313, 0x00, 10); // VEL_ILimit
    Can_Sdo_read_and_check(NodeId, 0x3313, 0x00);
    Can_Sdo_Write(NodeId, 0x3314, 0x00, 300); // VEL_Kvff
    Can_Sdo_read_and_check(NodeId, 0x3314, 0x00);
    Can_Sdo_Write(NodeId, 0x3315, 0x00, 100); // VEL_Kaff
    Can_Sdo_read_and_check(NodeId, 0x3315, 0x00);
    Can_Sdo_Write(NodeId, 0x3317, 0x00, 0); // VEL_KIxR
    Can_Sdo_read_and_check(NodeId, 0x3317, 0x00);

    Can_Sdo_Write(NodeId, 0x3321, 0x00, 2147483647); // VEL_LimitMaxPos
    Can_Sdo_read_and_check(NodeId, 0x3321, 0x00);
    Can_Sdo_Write(NodeId, 0x3323, 0x00, 2147483647); // VEL_LimitMaxNeg
    Can_Sdo_read_and_check(NodeId, 0x3323, 0x00);

    Can_Sdo_Write(NodeId, 0x3340, 0x00, 3000); // VEL_Acc_dV   1000
    Can_Sdo_read_and_check(NodeId, 0x3340, 0x00);
    Can_Sdo_Write(NodeId, 0x3341, 0x00, 1000); // VEL_Acc_dT
    Can_Sdo_read_and_check(NodeId, 0x3341, 0x00);
    Can_Sdo_Write(NodeId, 0x3342, 0x00, 3000); // VEL_Dec_dV   1000
    Can_Sdo_read_and_check(NodeId, 0x3342, 0x00);
    Can_Sdo_Write(NodeId, 0x3343, 0x00, 1000); // VEL_Dec_dT
    Can_Sdo_read_and_check(NodeId, 0x3343, 0x00);
    Can_Sdo_Write(NodeId, 0x3344, 0x00, 1000); // VEL_Dec_QuickStop_dV
    Can_Sdo_read_and_check(NodeId, 0x3344, 0x00);
    Can_Sdo_Write(NodeId, 0x3345, 0x00, 100); // VEL_Dec_QuickStop_dT
    Can_Sdo_read_and_check(NodeId, 0x3345, 0x00);
    Can_Sdo_Write(NodeId, 0x334c, 0x00, 1); // VEL_RampType
    Can_Sdo_read_and_check(NodeId, 0x334c, 0x00);

    Can_Sdo_Write(NodeId, 0x33c0, 0x00, 0); // VEL_BlockageGuarding_ConfigFlags
    Can_Sdo_read_and_check(NodeId, 0x33c0, 0x00);
    Can_Sdo_Write(NodeId, 0x33c0, 0x01, -1); // VEL_BlockageGuarding_VelLow
    Can_Sdo_read_and_check(NodeId, 0x33c0, 0x01);
    Can_Sdo_Write(NodeId, 0x33c0, 0x02, -1); // VEL_BlockageGuarding_VelHigh
    Can_Sdo_read_and_check(NodeId, 0x33c0, 0x02);
    Can_Sdo_Write(NodeId, 0x33c0, 0x03, 10); // VEL_BlockageGuarding_Time
    Can_Sdo_read_and_check(NodeId, 0x33c0, 0x03);

    Can_Sdo_Write(NodeId, 0x3502, 0x00, 1); // SVEL_ScaleNum
    Can_Sdo_read_and_check(NodeId, 0x3502, 0x00);
    Can_Sdo_Write(NodeId, 0x3503, 0x00, 1); // SVEL_ScaleDen
    Can_Sdo_read_and_check(NodeId, 0x3503, 0x00);
    Can_Sdo_Write(NodeId, 0x3504, 0x00, 1280); // SVEL_DesiredValue_Source
    Can_Sdo_read_and_check(NodeId, 0x3504, 0x00);
    Can_Sdo_Write(NodeId, 0x3505, 0x00, 1000); // SVEL_DesiredValue_Reference
    Can_Sdo_read_and_check(NodeId, 0x3505, 0x00);

    Can_Sdo_Write(NodeId, 0x3510, 0x00, 700); // SVEL_Kp
    Can_Sdo_read_and_check(NodeId, 0x3510, 0x00);
    Can_Sdo_Write(NodeId, 0x3511, 0x00, 50); // SVEL_Ki
    Can_Sdo_read_and_check(NodeId, 0x3511, 0x00);
    Can_Sdo_Write(NodeId, 0x3517, 0x00, 0); // SVEL_KIxR
    Can_Sdo_read_and_check(NodeId, 0x3517, 0x00);

    Can_Sdo_Write(NodeId, 0x3521, 0x00, 65535); // SVEL_LimitMaxPos
    Can_Sdo_read_and_check(NodeId, 0x3521, 0x00);
    Can_Sdo_Write(NodeId, 0x3523, 0x00, 65535); // SVEL_LimitMaxNeg
    Can_Sdo_read_and_check(NodeId, 0x3523, 0x00);

    Can_Sdo_Write(NodeId, 0x35a1, 0x00, 16383); // SVEL_MaxVelRange
    Can_Sdo_read_and_check(NodeId, 0x35a1, 0x00);

    Can_Sdo_Write(NodeId, 0x3720, 0x00, -2147483648); // POS_PositionLimitMin
    Can_Sdo_read_and_check(NodeId, 0x3720, 0x00);
    Can_Sdo_Write(NodeId, 0x3720, 0x01, 2147483647); // POS_PositionLimitMax
    Can_Sdo_read_and_check(NodeId, 0x3720, 0x01);

    Can_Sdo_Write(NodeId, 0x3732, 0x00, 1000); // POS_FollowingErrorWindow
    Can_Sdo_read_and_check(NodeId, 0x3732, 0x00);
    Can_Sdo_Write(NodeId, 0x3732, 0x01, 4294967295); // POS_FollowingErrorWindowDyn
    Can_Sdo_read_and_check(NodeId, 0x3732, 0x01);
    Can_Sdo_Write(NodeId, 0x3733, 0x00, 0); // POS_FollowingErrorTime
    Can_Sdo_read_and_check(NodeId, 0x3733, 0x00);
    Can_Sdo_Write(NodeId, 0x3733, 0x01, 65535); // POS_FollowingErrorTimeDyn
    Can_Sdo_read_and_check(NodeId, 0x3733, 0x01);
    Can_Sdo_Write(NodeId, 0x3734, 0x00, 4294967295); // POS_FollowingErrorLimit
    Can_Sdo_read_and_check(NodeId, 0x3734, 0x00);
    Can_Sdo_Write(NodeId, 0x373a, 0x00, 4294967295); // POS_PositionWindow
    Can_Sdo_read_and_check(NodeId, 0x373a, 0x00);
    Can_Sdo_Write(NodeId, 0x373b, 0x00, 0); // POS_PositionWindowTime
    Can_Sdo_read_and_check(NodeId, 0x373b, 0x00);
    Can_Sdo_Write(NodeId, 0x373b, 0x01, 65535); // POS_PositionWindowTimeout
    Can_Sdo_read_and_check(NodeId, 0x373b, 0x01);
    Can_Sdo_Write(NodeId, 0x373c, 0x00, 0); // POS_ReachedConfigFlags
    Can_Sdo_read_and_check(NodeId, 0x373c, 0x00);

    Can_Sdo_Write(NodeId, 0x374c, 0x00, 1); // POS_RampType
    Can_Sdo_read_and_check(NodeId, 0x374c, 0x00);

    Can_Sdo_Write(NodeId, 0x37a4, 0x01, 0); // POS_GearBacklashComp_Path
    Can_Sdo_read_and_check(NodeId, 0x37a4, 0x01);
    Can_Sdo_Write(NodeId, 0x37a4, 0x02, 100); // POS_GearBacklashComp_Vel
    Can_Sdo_read_and_check(NodeId, 0x37a4, 0x02);

    Can_Sdo_Write(NodeId, 0x37b2, 0x00, 0); // POS_HomingMethod
    Can_Sdo_read_and_check(NodeId, 0x37b2, 0x00);
    Can_Sdo_Write(NodeId, 0x37b3, 0x00, 0); // POS_HomingOffset
    Can_Sdo_read_and_check(NodeId, 0x37b3, 0x00);
    Can_Sdo_Write(NodeId, 0x37b4, 0x00, 0); // POS_HomingVelSwitch
    Can_Sdo_read_and_check(NodeId, 0x37b4, 0x00);
    Can_Sdo_Write(NodeId, 0x37b4, 0x01, 0); // POS_HomingVelZero
    Can_Sdo_read_and_check(NodeId, 0x37b4, 0x01);
    Can_Sdo_Write(NodeId, 0x37b5, 0x00, 60000); // POS_HomingAcc
    Can_Sdo_read_and_check(NodeId, 0x37b5, 0x00);
    Can_Sdo_Write(NodeId, 0x37b5, 0x01, 0); // POS_HomingDec
    Can_Sdo_read_and_check(NodeId, 0x37b5, 0x01);
    Can_Sdo_Write(NodeId, 0x37b6, 0x00, 2147483647); // POS_HomingMaxIndexPath
    Can_Sdo_read_and_check(NodeId, 0x37b6, 0x00);
    Can_Sdo_Write(NodeId, 0x37b6, 0x01, 0); // POS_HomingIndexOffset
    Can_Sdo_read_and_check(NodeId, 0x37b6, 0x01);
    Can_Sdo_Write(NodeId, 0x37b7, 0x00, 0); // POS_HomingBlock_ConfigFlags
    Can_Sdo_read_and_check(NodeId, 0x37b7, 0x00);
    Can_Sdo_Write(NodeId, 0x37b7, 0x01, 10); // POS_HomingBlock_VelLow
    Can_Sdo_read_and_check(NodeId, 0x37b7, 0x01);
    Can_Sdo_Write(NodeId, 0x37b7, 0x02, -1); // POS_HomingBlock_VelHigh
    Can_Sdo_read_and_check(NodeId, 0x37b7, 0x02);
    Can_Sdo_Write(NodeId, 0x37b7, 0x03, 1); // POS_HomingBlock_Time
    Can_Sdo_read_and_check(NodeId, 0x37b7, 0x03);
    Can_Sdo_Write(NodeId, 0x37b7, 0x04, -1); // POS_HomingBlock_FollowingErrorWindow
    Can_Sdo_read_and_check(NodeId, 0x37b7, 0x04);

    Can_Sdo_Write(NodeId, 0x39a0, 0x00, 0); // MOTOR_BrakeManagement_Config
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x00);
    Can_Sdo_Write(NodeId, 0x39a0, 0x06, -1); // MOTOR_BrakeManagement_VelMin
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x06);
    Can_Sdo_Write(NodeId, 0x39a0, 0x07, 0); // MOTOR_BrakeManagement_Din
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x07);
    Can_Sdo_Write(NodeId, 0x39a0, 0x08, -352); // MOTOR_BrakeManagement_Dout
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x08);
    Can_Sdo_Write(NodeId, 0x39a0, 0x09, 10000); // MOTOR_BrakeManagement_UpBrakeOff
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x09);
    Can_Sdo_Write(NodeId, 0x39a0, 0x0a, 8000); // MOTOR_BrakeManagement_UpBrakeOn
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x0a);
    Can_Sdo_Write(NodeId, 0x39a0, 0x10, 0); // MOTOR_BrakeManagement_OffDelay1
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x10);
    Can_Sdo_Write(NodeId, 0x39a0, 0x11, 0); // MOTOR_BrakeManagement_OffDelay2
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x11);
    Can_Sdo_Write(NodeId, 0x39a0, 0x12, 0); // MOTOR_BrakeManagement_OnDelay1
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x12);
    Can_Sdo_Write(NodeId, 0x39a0, 0x13, 0); // MOTOR_BrakeManagement_OnDelay2
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x13);
    Can_Sdo_Write(NodeId, 0x39a0, 0x18, 1); // MOTOR_BrakeManagement_OffOrConditionFlags
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x18);
    Can_Sdo_Write(NodeId, 0x39a0, 0x19, 0); // MOTOR_BrakeManagement_OffAndConditionFlags
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x19);
    Can_Sdo_Write(NodeId, 0x39a0, 0x1a, 1); // MOTOR_BrakeManagement_OnOrConditionFlags
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x1a);
    Can_Sdo_Write(NodeId, 0x39a0, 0x1b, 0); // MOTOR_BrakeManagement_OnAndConditionFlags
    Can_Sdo_read_and_check(NodeId, 0x39a0, 0x1b);

    Can_Sdo_Write(NodeId, 0x3b00, 0x00, 1); // FCT_Control
    Can_Sdo_read_and_check(NodeId, 0x3b00, 0x00);
    Can_Sdo_Write(NodeId, 0x3b00, 0x01, 0); // FCT_Precision
    Can_Sdo_read_and_check(NodeId, 0x3b00, 0x01);

    Can_Sdo_Write(NodeId, 0x3b10, 0x00, 0); // FCT_Polarity
    Can_Sdo_read_and_check(NodeId, 0x3b10, 0x00);
    Can_Sdo_Write(NodeId, 0x3b11, 0x00, 0); // FCT_PosNotationIndex
    Can_Sdo_read_and_check(NodeId, 0x3b11, 0x00);
    Can_Sdo_Write(NodeId, 0x3b12, 0x00, 180); // FCT_PosDimIndex
    Can_Sdo_read_and_check(NodeId, 0x3b12, 0x00);
    Can_Sdo_Write(NodeId, 0x3b13, 0x00, 0); // FCT_VelNotationIndex
    Can_Sdo_read_and_check(NodeId, 0x3b13, 0x00);
    Can_Sdo_Write(NodeId, 0x3b14, 0x00, 164); // FCT_VelDimIndex
    Can_Sdo_read_and_check(NodeId, 0x3b14, 0x00);
    Can_Sdo_Write(NodeId, 0x3b15, 0x00, 0); // FCT_AccNotationIndex
    Can_Sdo_read_and_check(NodeId, 0x3b15, 0x00);
    Can_Sdo_Write(NodeId, 0x3b16, 0x00, 176); // FCT_AccDimIndex
    Can_Sdo_read_and_check(NodeId, 0x3b16, 0x00);
    Can_Sdo_Write(NodeId, 0x3b17, 0x00, 1); // FCT_PosScaleNum
    Can_Sdo_read_and_check(NodeId, 0x3b17, 0x00);
    Can_Sdo_Write(NodeId, 0x3b17, 0x01, 1); // FCT_PosScaleDen
    Can_Sdo_read_and_check(NodeId, 0x3b17, 0x01);
    Can_Sdo_Write(NodeId, 0x3b18, 0x00, 1); // FCT_VelScaleNum
    Can_Sdo_read_and_check(NodeId, 0x3b18, 0x00);
    Can_Sdo_Write(NodeId, 0x3b18, 0x01, 1); // FCT_VelScaleDen
    Can_Sdo_read_and_check(NodeId, 0x3b18, 0x01);
    Can_Sdo_Write(NodeId, 0x3b19, 0x00, 1); // FCT_GearRatio_MotorRev
    Can_Sdo_read_and_check(NodeId, 0x3b19, 0x00);
    Can_Sdo_Write(NodeId, 0x3b19, 0x01, 1); // FCT_GearRatio_ShaftRev
    Can_Sdo_read_and_check(NodeId, 0x3b19, 0x01);
    Can_Sdo_Write(NodeId, 0x3b1a, 0x00, 1); // FCT_FeedConstant_Feed
    Can_Sdo_read_and_check(NodeId, 0x3b1a, 0x00);
    Can_Sdo_Write(NodeId, 0x3b1a, 0x01, 1); // FCT_FeedConstant_ShaftRev
    Can_Sdo_read_and_check(NodeId, 0x3b1a, 0x01);

    //////////////////////////////////////////
    // CAN PDO's Mapping //
    //////////////////////////////////////////

    ////////// 1st TX-PDO //////////
    // TXCobId = 0x180 + NodeId;                     // CobId for the first TX-PDO (motor is sending)
    // RXCobId = 0x200 + NodeId;                     // CobId for the first RX-PDO (motor is receiving)
    Can_Sdo_Write(NodeId, 0x1A00, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A00, 0);
    Can_Sdo_Write(NodeId, 0x1A00, 1, 0x37620020); // set first object (Actual position inc.):
    // 08 = 1 byte, 10 = 2 bytes, 20 = 4 bytes
    Can_Sdo_read_and_check(NodeId, 0x1A00, 1);
    Can_Sdo_Write(NodeId, 0x1A00, 2, 0x32620120); // set second object (Filtered Current current in mA):
    Can_Sdo_read_and_check(NodeId, 0x1A00, 2);

    Can_Sdo_Write(NodeId, 0x1A00, 0, 2); // write numbers of objects into the table = 2
    Can_Sdo_read_and_check(NodeId, 0x1A00, 0);

    Can_Sdo_Write(NodeId, 0x1800, 2, 1); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1800, 2);
    Can_Sdo_Write(NodeId, 0x1800, 3, 100); // inhibit time in 100µs
    Can_Sdo_read_and_check(NodeId, 0x1800, 3);

    ////////// 2nd TX-PDO //////////
    // TXCobId = 0x280 + NodeId;                     // CobId for the second TX-PDO (motor is sending     2
    // RXCobId = 0x300 + NodeId;                     // CobId for the second RX-PDO (motor is receiving)   2
    Can_Sdo_Write(NodeId, 0x1A01, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A01, 0);
    Can_Sdo_Write(NodeId, 0x1A01, 1, 0x37610020); // Actual Command Position
    Can_Sdo_read_and_check(NodeId, 0x1A01, 1);
    Can_Sdo_Write(NodeId, 0x1A01, 2, 0x37600020); // set third object (Target position):
    Can_Sdo_read_and_check(NodeId, 0x1A01, 2);
    Can_Sdo_Write(NodeId, 0x1A01, 0, 2); // write numbers of objects into the table = 2
    Can_Sdo_read_and_check(NodeId, 0x1A01, 0);
    Can_Sdo_Write(NodeId, 0x1801, 2, 1); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1801, 2);
    Can_Sdo_Write(NodeId, 0x1801, 3, 100); // inhibit time in 100µs
    Can_Sdo_read_and_check(NodeId, 0x1801, 3);

    ////////// 3rd TX-PDO //////////
    // TXCobId = 0x380 + NodeId;                     // CobId for the second TX-PDO (motor is sending)
    // RXCobId = 0x400 + NodeId;                     // CobId for the second RX-PDO (motor is receiving)
    Can_Sdo_Write(NodeId, 0x1A02, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A02, 0);
    Can_Sdo_Write(NodeId, 0x1A02, 1, 0x3A040120); // set 1st object (Actual Velocity in RPM)
    Can_Sdo_read_and_check(NodeId, 0x1A02, 1);
    Can_Sdo_Write(NodeId, 0x1A02, 2, 0x35000020); // set 2nd object (SVEl des (RPM)
    Can_Sdo_read_and_check(NodeId, 0x1A02, 2);
    Can_Sdo_Write(NodeId, 0x1A02, 0, 2); // write numbers of objects into the table = 2
    Can_Sdo_read_and_check(NodeId, 0x1A02, 0);
    Can_Sdo_Write(NodeId, 0x1802, 2, 1); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1802, 2);
    Can_Sdo_Write(NodeId, 0x1802, 3, 100); // inhibit time in 100µs
    Can_Sdo_read_and_check(NodeId, 0x1802, 3);

    ////////// 4th TX-PDO //////////
    // TXCobId = 0x480 + NodeId_DK                     // CobId for the second TX-PDO (motor is sending)
    // RXCobId = 0x500 + NodeId_DK                     // CobId for the second RX-PDO (motor is receiving)
    Can_Sdo_Write(NodeId, 0x1A03, 0, 0); // delete mapping table first
    Can_Sdo_read_and_check(NodeId, 0x1A03, 0);
    Can_Sdo_Write(NodeId, 0x1A03, 1, 0x31000010); // set 1st object (Analog Input 0 in mv remmenber NEGATIVE):
    Can_Sdo_read_and_check(NodeId, 0x1A03, 1);
    Can_Sdo_Write(NodeId, 0x1A03, 2, 0x31110020); // set 2nd object (Motor Voltage (mV)
    Can_Sdo_read_and_check(NodeId, 0x1A03, 2);
    Can_Sdo_Write(NodeId, 0x1A03, 0, 2); // write numbers of objects into the table = 2
    Can_Sdo_read_and_check(NodeId, 0x1A03, 0);
    Can_Sdo_Write(NodeId, 0x1803, 2, 1); // asynchronous (see above)
    Can_Sdo_read_and_check(NodeId, 0x1803, 2);
    Can_Sdo_Write(NodeId, 0x1803, 3, 100); // inhibit time in 100µs
    Can_Sdo_read_and_check(NodeId, 0x1803, 3);

    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);
    //-----------------------------------------------------------------------------------------------------------------------
    // Device mode
    Can_Sdo_Write(NodeId, 0x3003, 0, 7); // Device mode "position mode"
    Can_Sdo_read_and_check(NodeId, 0x3003, 0);

    // sleep for 5 miliseconds to wait the bus to be empty
    /**
     * @todo try to delete this usleep sometime
     */
    // usleep(5 * 1000);

    // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Can_Sdo_Write(NodeId, 0x3000, 0, 1); // Reset error register
    Can_Sdo_read_and_check(NodeId, 0x3000, 0);
    Can_Sdo_Write(NodeId, 0x3004, 0, 1); // Enable power stage
    Can_Sdo_read_and_check(NodeId, 0x3004, 0);
    usleep(100 * 1000);                        // 100ms delay for power stage to be enabled

    
    Can_Sdo_Write(NodeId, 0x3300, 0x00, 400); // VEL_DesiredValue [rpm]
    Can_Sdo_read_and_check(NodeId, 0x3300, 0x00);

    start_Pdo_logging();
    uint8_t pdo_data_1[2] = {0x01, NodeId};
    Can_Pdo_Write(0, pdo_data_1, sizeof(pdo_data_1) / sizeof(pdo_data_1[0]));
 
    //Can_Sdo_re-ad_and_check_while_Pdo_logging(NodeId, 0x3790, 0);
    Can_Sdo_Write(NodeId, 0x3790, 0, 1000);
    //Can_Sdo_write_while_Pdo_logging(NodeId, 0x3790, 0, 1000); // go to desired  position in increments
    printf("go to release\n");
    

    for(int i = 0 ; i<200;i++){
        Can_Sdo_Write_NULL(0x80);
        usleep(20 * 1000);
    }
    sleep(2);

    
    Can_Sdo_Write(NodeId, 0x1005, 0x00, 0x80);

    Can_Sdo_Write(NodeId, 0x3790, 0, 0);
    //Can_Sdo_write_while_Pdo_logging(NodeId, 0x3790, 0, 0); // go to desired  position in increments
    for(int i = 0 ; i<200;i++){
        Can_Sdo_Write_NULL(0x80);
        usleep(20 * 1000);
    }
    printf("go to full close\n");
    sleep(2);

    


    /// uint8_t pdo_data_2[2] = {0x80, NodeId};
    stop_Pdo_logging();
    Can_Pdo_Write(0, pdo_data_2, sizeof(pdo_data_2) / sizeof(pdo_data_2[0]));
    Can_Sdo_Write(NodeId, 0x3004, 0, 0); // Disable power stage
    /**
     * @brief uncomment this later
     *
     */
    // Can_Sdo_read_and_check(NodeId, 0x4004, 1);

    /**
     * @brief clean up
     */
    //printf("Program run time: %f  \r\n", (double)clock() / (CLOCKS_PER_SEC));

    printf("Demo finished!\n");

    close_can();

    return 0;
}
