#include "can_lib.h"

/*Global Variables*/
#define TIMEOUT_THRESHOLD 0.02 // Threshold for timeou in seconds.
int s;
struct can_frame frame;
clock_t clk_t;
pthread_t th_timer, pdo_logging;

int sdo_write_ok = 1;

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

    // pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    clk_t = clock();
    while (1)
    {

        if ((double)(clock() - clk_t) / (CLOCKS_PER_SEC) > TIMEOUT_THRESHOLD)
        {
            /**
             * @todo later this can be changed by a more specific error message.
             */

            printf("--------TIME OUT ERROR--------\r\n");
            printf("No response from the CAN bus for %f seconds\r\n", ((double)(clock() - clk_t) / (CLOCKS_PER_SEC)));
            // printf("current time: %f and (double)(clock() - clk_t) : %f \r\n", (double)clock() / (CLOCKS_PER_SEC),(double)(clock() - clk_t)/ (CLOCKS_PER_SEC));
            // printf("---TIME OUT ERROR---\r\n");

            exit(1);
        }
    }
}

void Can_Sdo_read_and_check(uint16_t can_id, uint16_t addr, uint8_t sub_addr)
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

            /* Print the CAN message information */
            printf("can_id = 0x%X\r\ncan_dlc = %d \r\n", frame.can_id & 0x1FFFFFFF, frame.can_dlc);
            for (int i = 0; i < 8; i++)
                printf("data[%d] = %d\r\n", i, frame.data[i]);

            /* Check if the response from the motor is correct.
                if the can_id is not correct
             */

            if (frame.can_id != can_id + 0x580 ||
                frame.data[0] != 0x60 ||
                frame.data[1] != (addr & (0xff)) ||
                frame.data[2] != (addr >> 8) ||
                frame.data[3] != sub_addr)
            {

                // printf("frame.data[1]: %x, addr & (0xff): %x", frame.data[1], addr & (0xff));
                printf("--------MOTOR RESPONSE ERROR--------\r\n"
                       "The response from the motor should be (in hex form): \r\n"
                       "CAN ID: %x, CAN byte 0: %x for an bug-free motor response\r\n",
                       (can_id + 0x580), 0x60);

                printf("Address should be: %x\r\n", addr);
                printf("Sub address should be: %x\r\n", sub_addr);
                printf("\r\n But actual the motor response is: \r\n"
                       "CAN ID: %x, CAN byte 0: %x\r\n",
                       frame.can_id, frame.data[0]);

                printf("Actual address response is : %x%x\r\n", frame.data[2], frame.data[1]);
                printf("Actual sub address response is: %x\r\n", frame.data[3]);
                exit(1);
            };

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

void *Pdo_logging_thread(void *args)
{

    // pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    // pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    int logging_number = 0;
    while (1)
    {

        int nbytes = read(s, &frame, sizeof(frame));
        if (nbytes > 0)
        {
            if (!(frame.can_id & CAN_EFF_FLAG))
                // if(frame.can_id&0x80000000==0)
                printf("Received standard frame!\n");
            else
                printf("Received extended frame!\n");

            /* Print the CAN message information */
            printf("can_id = 0x%X\r\ncan_dlc = %d \r\n", frame.can_id & 0x1FFFFFFF, frame.can_dlc);
            for (int i = 0; i < 8; i++)
                printf("data[%d] = %d\r\n", i, frame.data[i]);

            // If the one batch of the motor data is recorded,
            // the flag sdo_write_ok is set to 1
            sdo_write_ok = (++logging_number % 4) ? 0 : 1;
        }
    }
}

void start_Pdo_logging()
{
    pthread_create(&pdo_logging, NULL, Pdo_logging_thread, NULL);
}

void stop_Pdo_logging()
{
    pthread_cancel(pdo_logging);
}

void Can_Sdo_write_while_Pdo_logging(uint16_t can_id, uint16_t addr, uint8_t sub_addr, uint32_t data)
{

    while (1)
    {
        /* After a whole batch of Pdo data is received */
        if (sdo_write_ok)
        {
            // Stop the Pdo logging and write the Sdo message
            stop_Pdo_logging();
            Can_Sdo_Write(can_id, addr, sub_addr, data);
            return NULL;
        }
    }
}