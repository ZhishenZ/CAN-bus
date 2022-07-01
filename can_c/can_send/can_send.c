/*This is a socket CAN transmiter test programmer*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>


int can_send_init(){


    return 0;
}


int main()
{
    int ret;
    int s, nbytes;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    memset(&frame, 0, sizeof(struct can_frame));

    // close can device before setting the bitrate
    system("sudo ifconfig can0 down");
    // set the bit rate to 500000
	system("sudo ip link set can0 type can bitrate 500000");
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

    /*assembly  message data! */
    frame.can_id = 0x67D;
    frame.can_dlc = 8;
    frame.data[0] = 0x23;
    frame.data[1] = 0x32;
    frame.data[2] = 0x47;
    frame.data[3] = 0x01;
    frame.data[4] = 0x7B;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;
    //if(frame.can_id&CAN_EFF_FLAG==0)

    // if(!(frame.can_id&CAN_EFF_FLAG))
	// 	printf("Transmit standard frame!\n");
	// else
	// 	printf("Transmit extended frame!\n");
    // printf("can_id  = 0x%X\r\n", frame.can_id);
    // printf("can_dlc = %d\r\n", frame.can_dlc);
    // int i = 0;
    // for(i = 0; i < 8; i++)
    //     printf("data[%d] = %d\r\n", i, frame.data[i]);
    
    /*Send message out */
    nbytes = write(s, &frame, sizeof(frame)); 
    if(nbytes != sizeof(frame)) {
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }
    
    /*Close the socket and can0 */
    close(s);
    system("sudo ifconfig can0 down");
    return 0;
}
