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


/**
 * @brief initialization for the can send function for can0
 *        here struct ifreq ifr and sockaddr_can addr are not passed by pointer
 *        because they are only used once in this function 
 * 
 * @param frame the can frame to be set to zero 
 * @param s  
 * @param ifr 
 * @param addr 
 * @return int 
 */

int can_send_init(struct can_frame frame, int *s, struct ifreq ifr, struct sockaddr_can addr){

    int ret;
    memset(&frame, 0, sizeof(struct can_frame));
    // close can device before setting the bitrate
    system("sudo ifconfig can0 down");
    // set the bit rate to 500000
	system("sudo ip link set can0 type can bitrate 500000");
    system("sudo ifconfig can0 up");

    /*Create socket*/
    *s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (*s < 0) {
        perror("Create socket PF_CAN failed!");
        return 1;
    }

    /*Specify can0 device*/
    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(*s, SIOCGIFINDEX, &ifr);
    if (ret < 0) {
        perror("ioctl interface index failed!");
        return 1;
    }

    /*Bind the socket to can0*/
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(*s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind failed!");
        return 1;
    }

    /*Disable filtering rules,this program only send message do not receive packets */
    setsockopt(*s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    return 0;
}

/**
 * @brief send the data to CAN bus
 * 
 * @param s 
 * @param frame 
 * @param can_id 
 * @param data 
 * @param data_len 
 */
void send_data( int s, struct can_frame frame, int can_id, uint8_t* data, uint8_t data_len){

    int i;
    /*assembly  message data! */
    frame.can_id = can_id;
    frame.can_dlc = data_len;
    for(i=0;i<data_len;i++){
        frame.data[i] = data[i];
    }

    i = write(s, &frame, sizeof(frame)); 
    if(i != sizeof(frame)) {
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }

}

/**
 * @brief close and clean up 
 * 
 * @param s 
 */
void close_can_send(int *s){

    /*Close the socket and can0 */
    close(*s);
    system("sudo ifconfig can0 down");

}


int main()
{

    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    
    // initialization
    if (can_send_init(frame, &s,ifr, addr))
        return 1;


    
    uint8_t data[ ] = {0x23, 0x32, 0x47, 0x01, 0x7B, 0x00, 0x00, 0x00};
    uint8_t data_len = sizeof(data)/sizeof(data[0]);
    int ID = 0x670;

    send_data(s, frame, ID, data, data_len);

    data[0] = 1;
    data[1] = 2;
    send_data(s, frame, 1, data , 5);  


    close_can_send(&s);

    return 0;
}
