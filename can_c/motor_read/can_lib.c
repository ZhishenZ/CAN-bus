#include "can_lib.h"

/*Global Variables*/
#define TIMEOUT_THRESHOLD 0.5 // Threshold for timeou in seconds. 
int s;
struct can_frame frame;
clock_t clk_t;
pthread_t th_timer;


int can_send_init(struct ifreq ifr, struct sockaddr_can addr)
{

    int ret;
    memset(&frame, 0, sizeof(struct can_frame));
    // close can device before setting the bitrate
    system("sudo ifconfig can0 down");
    // set the bit rate to 500000
    system("sudo ip link set can0 type can bitrate 1000000");
    system("sudo ifconfig can0 up");

    /*Create socket*/
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (s < 0)
    {
        perror("Create socket PF_CAN failed!");
        return 1;
    }

    /*Specify can0 device*/
    strcpy(ifr.ifr_name, "can0");
    ret = ioctl(s, SIOCGIFINDEX, &ifr);
    if (ret < 0)
    {
        perror("ioctl interface index failed!");
        return 1;
    }

    /*Bind the socket to can0*/
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    ret = bind(s, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0)
    {
        perror("bind failed!");
        return 1;
    }

    /*Disable filtering rules,this program only send message do not receive packets */
    /* since we also need to receive message from the CAN bus, we do not set any filter. */
    // setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    return 0;
}

void close_can()
{
    
    /*Close the socket and can0 */
    // pthread_cancel(th_timer);
    close(s);
    system("sudo ifconfig can0 down");
}

void Can_Sdo_Write(uint16_t can_id, uint16_t addr, uint8_t sub_addr, uint32_t data)
{
    int ret;
    /*assembly  message data! */
    frame.can_id = can_id + 0x600;
    frame.can_dlc = 8;
    // SDO write
    frame.data[0] = 0x23;

    // address
    frame.data[1] = addr & (0xff);
    frame.data[2] = addr >> 8;

    // sub address
    frame.data[3] = sub_addr;

    // data
    frame.data[4] = data & 0xff;
    frame.data[5] = (data >> 8) & (0xff);
    frame.data[6] = (data >> 16) & (0xff);
    frame.data[7] = data >> 24;

    ret = write(s, &frame, sizeof(frame));
    if (ret != sizeof(frame))
    {
        printf("ret: %d sizeof(frame): %d\n", ret, sizeof(frame));
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }
}

void Can_Sdo_Read(uint16_t can_id, uint16_t addr, uint8_t sub_addr)
{
    int ret;
    /*assembly  message data! */
    frame.can_id = can_id + 0x600;
    frame.can_dlc = 8;
    // SDO Read
    frame.data[0] = 0x40;

    // address
    frame.data[1] = addr & (0xff);
    frame.data[2] = addr >> 8;

    // sub address
    frame.data[3] = sub_addr;

    // data
    frame.data[4] = 0;
    frame.data[5] = 0;
    frame.data[6] = 0;
    frame.data[7] = 0;

    ret = write(s, &frame, sizeof(frame));
    if (ret != sizeof(frame))
    {
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }
}

void Can_Pdo_Write(uint16_t can_id, uint8_t *data_array, uint8_t array_len)
{

    frame.can_id = can_id;
    frame.can_dlc = array_len;
    for (int i = 0; i < array_len; i++)
    {
        frame.data[i] = data_array[i];
    }
    int ret = write(s, &frame, sizeof(frame));
    if (ret != sizeof(frame))
    {
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }
}

void *time_watcher(void *args)
{   
    
    //pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    clk_t = clock();
    while (1)
    {

        if ((double)(clock() - clk_t) / (CLOCKS_PER_SEC) > TIMEOUT_THRESHOLD)
        {
            /**
             * @todo later this can be changed by a more specific error message.
             */
            
            printf("--------TIME OUT ERROR--------\r\n");
            printf("No response from the CAN bus for %f seconds\r\n",((double)(clock() - clk_t) / (CLOCKS_PER_SEC)));
            // printf("current time: %f and (double)(clock() - clk_t) : %f \r\n", (double)clock() / (CLOCKS_PER_SEC),(double)(clock() - clk_t)/ (CLOCKS_PER_SEC));
            // printf("---TIME OUT ERROR---\r\n");
            

            exit(1);
        }
        
    }
}








void Can_read_and_check()
{

    // pthread_t th_timer;

    pthread_create(&th_timer, NULL, time_watcher, NULL);

    while (1)
    {


        int nbytes = read(s, &frame, sizeof(frame));

        // after receiving the CAN message, kill the watch_timer_thread 
        pthread_cancel(th_timer);
        

        if (nbytes > 0)
        {
            if (!(frame.can_id & CAN_EFF_FLAG))
                // if(frame.can_id&0x80000000==0)
                printf("Received standard frame!\n");
            else
                printf("Received extended frame!\n");
            printf("can_id = 0x%X\r\ncan_dlc = %d \r\n", frame.can_id & 0x1FFFFFFF, frame.can_dlc);
            for (int i = 0; i < 8; i++)
                printf("data[%d] = %d\r\n", i, frame.data[i]);

            
            
            break;
            // mask below sentense to receive all the time other wise can only receive one time!
            // break;
        }

        // printf("Time measured in seconds: %f \n\r", (double)(clock() - clk_t)/(CLOCKS_PER_SEC));
        // // Check TIME OUT error
        // if ((double)(clock() - clk_t)/(CLOCKS_PER_SEC) > TIMEOUT_THRESHOLD)
        // {   
        //     /**
        //      * @todo later this can be changed by a more specific error message. 
        //      */
        //     printf("TIME OUT ERROR\r\n");
        // }
    }
}

