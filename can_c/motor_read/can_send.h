#ifndef CAN_SEND_H
#define CAN_SEND_H

/*Global Variables*/

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

extern int s;
extern struct can_frame frame;

int can_send_init( struct ifreq ifr, struct sockaddr_can addr); 

void close_can();

void Can_Sdo_Write( uint16_t can_id, uint16_t addr, uint8_t sub_addr, uint32_t data);

void Can_Sdo_Read( uint16_t can_id, uint16_t addr, uint8_t sub_addr);

#endif // CAN_SEND_H