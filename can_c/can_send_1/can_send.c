/**
 * @file can_send.c
 * @author Zhishen Z. (zzsubt@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-01
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

/* global variables*/
int s;
struct can_frame frame;

/**
 * @brief initialization for the can send function for can0
 *        here struct ifreq ifr and sockaddr_can addr are not passed by pointer
 *        because they are only used once in this function 
 * 
 * @param ifr 
 * @param addr 
 * @return int 
 */

int can_send_init( struct ifreq ifr, struct sockaddr_can addr){

    int ret;
    memset(&frame, 0, sizeof(struct can_frame));
    // close can device before setting the bitrate
    system("sudo ifconfig can0 down");
    // set the bit rate to 500000
	system("sudo ip link set can0 type can bitrate 1000000");
    system("sudo ifconfig can0 up");

    /*Create socket*/
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0) {
        perror("Create socket PF_CAN failed!");
        return 1;
    }

    /*Specify can0 device*/
    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(s, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl interface index failed!");
        return 1;
    }

    /*Bind the socket to can0*/
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind failed!");
        return 1;
    }

    /*Disable filtering rules,this program only send message do not receive packets */
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    return 0;
}


void close_can(){

    /*Close the socket and can0 */
    close(s);
    system("sudo ifconfig can0 down");

}


void Can_Sdo_Write( uint16_t can_id, uint16_t addr, uint8_t sub_addr, uint32_t data){
    int ret;
    /*assembly  message data! */
    frame.can_id = can_id + 0x600;
    frame.can_dlc = 8;
    //SDO write
    frame.data[0] = 0x23;

    //address
    frame.data[1] = addr & (0xff);
    frame.data[2] = addr >> 8;

    //sub address
    frame.data[3] = sub_addr;

    //data
    frame.data[4] = data & 0xff;
    frame.data[5] = (data >> 8) & (0xff);
    frame.data[6] = (data >> 16) & (0xff);
    frame.data[7] = data >> 24;

    ret = write(s, &frame, sizeof(frame)); 
    if(ret != sizeof(frame)) {
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }

}


void Can_Sdo_Read( uint16_t can_id, uint16_t addr, uint8_t sub_addr){
    int ret;
    /*assembly  message data! */
    frame.can_id = can_id + 0x600;
    frame.can_dlc = 8;
    //SDO Read
    frame.data[0] = 0x40;

    //address
    frame.data[1] = addr & (0xff);
    frame.data[2] = addr >> 8;

    //sub address
    frame.data[3] = sub_addr;

    //data
    frame.data[4] = 0;
    frame.data[5] = 0;
    frame.data[6] = 0;
    frame.data[7] = 0;

    ret = write(s, &frame, sizeof(frame)); 
    if(ret != sizeof(frame)) {
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }

}




int main()
{

   
    struct sockaddr_can addr;
    struct ifreq ifr;
    // initialization
    if (can_send_init(ifr, addr))
        return 1;

    Can_Sdo_Write( 127, 0x4732, 1, 123);
    Can_Sdo_Write( 127, 0x473B, 2, 456);
    Can_Sdo_Write( 127, 0x4310, 1, 10000);
    Can_Sdo_Write( 127, 0x4310, 1, 100000);


    Can_Sdo_Read( 127, 0x4310, 1);

    close_can();

    return 0;
}
