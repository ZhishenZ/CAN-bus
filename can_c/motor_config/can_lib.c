#include "can_lib.h"

/*Global Variables*/
#define TIMEOUT_THRESHOLD 0.2 // Threshold for timeou in seconds. before was 0.02
#define LOGGING_FILE_NAME "Pdo_response.csv"
int s;
int logging_number = 0;
int sdo_write_ok = 1;
struct can_frame frame;
clock_t clk_t;
struct timeval tv;
pthread_t th_timer, pdo_logging;
FILE *log_file_p;
char buf[32];
char long_buf[128];


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
    fclose(log_file_p);
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
    //usleep(10000);
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
            printf("No response from the motor via bus for %f seconds\r\n", ((double)(clock() - clk_t) / (CLOCKS_PER_SEC)));
            exit(1);
        }
    }
}

void Can_Sdo_read_and_check_while_Pdo_logging(uint16_t can_id, uint16_t addr, uint8_t sub_addr)
{

    // pthread_t th_timer;
    int i, nbytes;

    pthread_create(&th_timer, NULL, time_watcher, NULL);

    while (1)
    {
        nbytes = read(s, &frame, sizeof(frame));
        // after receiving the CAN message, kill the watch_timer_thread
        pthread_cancel(th_timer);

        if (nbytes > 0)
        {
            if (frame.can_id == 0x1FF ||
                frame.can_id == 0x2FF ||
                frame.can_id == 0x3FF ||
                frame.can_id == 0x4FF)
            {
                gettimeofday(&tv, NULL);
                strftime(buf, 32, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));

                sprintf(long_buf, "%s.%06d,%X,%d,",
                        buf, tv.tv_usec, frame.can_id, frame.can_dlc);
                // memset(buf,'\0',32);
                for (i = 0; i < frame.can_dlc; i++)
                {
                    sprintf(buf, " %02X", frame.data[i]);
                    strcat(long_buf, buf);
                }
                strcat(long_buf, "\n");

                /**
                 * @todo delete this later
                 *       this is only for testing
                 */
                if (logging_number == 3)
                    strcat(long_buf, "\n");

                // store data to the CSV file
                fprintf(log_file_p, long_buf);

                sdo_write_ok = (++logging_number % 4) ? 0 : 1;
                logging_number = (logging_number == 4) ? 0 : logging_number;
            }
            else if (frame.can_id != can_id + 0x580 ||
                     frame.data[0] != 0x60 ||
                     frame.data[1] != (addr & (0xff)) ||
                     frame.data[2] != (addr >> 8) ||
                     frame.data[3] != sub_addr)
            {

                printf("--------MOTOR RESPONSE ERROR--------\r\n"
                       "The response from the motor should be (in hex form): \r\n"
                       "CAN ID: %X, CAN byte 0: %X for an bug-free motor response\r\n",
                       (can_id + 0x580), 0x60);

                printf("Address should be: %X\r\n", addr);
                printf("Sub address should be: %X\r\n", sub_addr);
                printf("\r\n But actual the motor response is: \r\n"
                       "CAN ID: %X, CAN byte 0: %X\r\n",
                       frame.can_id, frame.data[0]);

                printf("Actual address response is : %X%X\r\n", frame.data[2], frame.data[1]);
                printf("Actual sub address response is: %X\r\n", frame.data[3]);
                exit(1);
            }
            else
            {
                break;
            }

            // mask below sentense to receive all the time other wise can only receive one time!
            // break;
        }
    }
}

void Can_Sdo_read_and_check(uint16_t can_id, uint16_t addr, uint8_t sub_addr)
{

    pthread_create(&th_timer, NULL, time_watcher, NULL);

    while (1)
    {

        int nbytes = read(s, &frame, sizeof(frame));

        // after receiving the CAN message, kill the watch_timer_thread
        pthread_cancel(th_timer);

        if (nbytes > 0)
        {

            if (frame.can_id != can_id + 0x580 ||
                frame.data[0] != 0x60 ||
                frame.data[1] != (addr & (0xff)) ||
                frame.data[2] != (addr >> 8) ||
                frame.data[3] != sub_addr)
            {

                printf("--------MOTOR RESPONSE ERROR--------\r\n"
                       "The response from the motor should be (in hex form): \r\n"
                       "CAN ID: %X, CAN byte 0: %X for an bug-free motor response\r\n",
                       (can_id + 0x580), 0x60);

                printf("Address should be: %X\r\n", addr);
                printf("Sub address should be: %X\r\n", sub_addr);
                printf("\r\n But actual the motor response is: \r\n"
                       "CAN ID: %X, CAN byte 0: %X\r\n",
                       frame.can_id, frame.data[0]);

                printf("Actual address response is : %X%X\r\n", frame.data[2], frame.data[1]);
                printf("Actual sub address response is: %X\r\n", frame.data[3]);
                exit(1);
            }

            break;
        }
    }
}

void *Pdo_logging_thread(void *args)
{

    // pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    int i, nbytes;
    // pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    while (1)
    {
        nbytes = read(s, &frame, sizeof(frame));
        if (nbytes > 0 && (frame.can_id == 0x1FF ||
                           frame.can_id == 0x2FF ||
                           frame.can_id == 0x3FF ||
                           frame.can_id == 0x4FF))
        {

            gettimeofday(&tv, NULL);
            strftime(buf, 32, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));

            sprintf(long_buf, "%s.%06d,%X,%d,",
                    buf, tv.tv_usec, frame.can_id, frame.can_dlc);
            // memset(buf,'\0',32);
            for (i = 0; i < frame.can_dlc; i++)
            {
                sprintf(buf, " %02X", frame.data[i]);
                strcat(long_buf, buf);
            }
            // strcat(buf,"\n");
            // fprinf(log_file_p,buf);
            strcat(long_buf, "\n");

            /**
             * @todo delete this later
             *
             */
            if (logging_number == 3)
                strcat(long_buf, "\n");

            fprintf(log_file_p, long_buf);

            // set the sdo write ok if four messages are received.
            sdo_write_ok = (++logging_number % 4) ? 0 : 1;
            logging_number = (logging_number == 4) ? 0 : logging_number;
        }
    }
}

void start_Pdo_logging()
{   
    if(pthread_create(&pdo_logging, NULL, Pdo_logging_thread, NULL)!=0){
        printf("--------PDO LOGGING FAILED TO START--------\r\n");
    };
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
            break;
        }
    }
}

int create_log_file()
{

    log_file_p = fopen(LOGGING_FILE_NAME, "w");
    int write_status = fprintf(log_file_p, "Local Time,ID (hex),DLC,Data(hex)\n");
    if (write_status < 0)
    {
        printf("Error when creating log file. \n");
        return 1;
    }

    return 0;
}