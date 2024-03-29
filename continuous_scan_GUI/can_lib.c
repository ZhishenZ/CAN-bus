#include "can_lib.h"

/*Global Variables*/
#define TIMEOUT_THRESHOLD 0.2 // Threshold for timeou in seconds. before was 0.02
#define LOGGING_FILE_FOLDER "Log_Files_PDO/"
int s;
int logging_number = 0;
int sdo_write_ok = 1;
int pdo_timeout = 0;
struct can_frame frame;
clock_t clk_t;
struct timeval tv, tv_start;
pthread_t th_timer, pdo_logging;
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
    gettimeofday(&tv_start, NULL);
    /*Disable filtering rules,this program only send message do not receive packets */
    /* since we also need to receive message from the CAN bus, we do not set any filter. */
    // setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    return 0;
}

void close_can(FILE *file_ptr)
{
    /*Close the socket and can0 */
    fclose(file_ptr);
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
    // usleep(10000);
    if (ret != sizeof(frame))
    {
        printf("ret: %d sizeof(frame): %d\n", ret, sizeof(frame));
        printf("Send  frame incompletely!\r\n");
        system("sudo ifconfig can0 down");
    }
}

void Can_Sdo_Write_NULL(uint16_t can_id)
{
    /*assembly  message data! */
    struct can_frame frame_;
    frame_.can_id = can_id;
    frame_.can_dlc = 0;
    int ret = write(s, &frame_, sizeof(frame_));
    // usleep(10000);
    if (ret != sizeof(frame_))
    {
        printf("ret: %d sizeof(frame): %d\n", ret, sizeof(frame_));
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

FILE* create_log_file(FILE *log_file_p)
{
    // set the pathLogFiles
    char path_LogFiles[128];
    struct tm *current_tm;
    time_t this_time_t;

    char time_string[32];
    char log_filename_device[512] = "";

    getcwd(path_LogFiles, sizeof(path_LogFiles));
    strcat(path_LogFiles, "/Log_Files_PDO/");

    // Initialize the output file
    this_time_t = time(NULL);
    current_tm = localtime(&this_time_t);
    // csv file name
    sprintf(time_string, "%02d_%02d_%02d_%02d_%02d_%02d_%s.csv", current_tm->tm_year + 1900,
            (current_tm->tm_mon + 1), current_tm->tm_mday, current_tm->tm_hour,
            current_tm->tm_min, current_tm->tm_sec, current_tm->tm_zone);

    strcat(log_filename_device, path_LogFiles);
    strcat(log_filename_device, time_string);

    // Open the log file
    log_file_p = fopen(log_filename_device, "w");
    fprintf(log_file_p, "Local Time,Program run time (ms),ID (hex),DLC,Data(hex)\n");

    return log_file_p;
}

void *pdo_time_watcher(void *args)
{
    // pthread_setcancelstate(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    clk_t = clock();
    while (1)
    {

        if ((double)(clock() - clk_t) / (CLOCKS_PER_SEC) > TIMEOUT_THRESHOLD)
        {
            // we do not have to stop the program if there is a timeout
            pdo_timeout = 1;
        }
    }
}

void get_Pdo_response(uint32_t *motor_current,
                      uint32_t *motor_position,
                      uint32_t *load_cell_voltage)
{
    int check_4_pdos[4] = {0, 0, 0, 0};
    int nbytes;

    pthread_create(&th_timer, NULL, pdo_time_watcher, NULL);
    while (1)
    {

        /*check if the four PDOs are all updated*/
        if ((check_4_pdos[0] && check_4_pdos[1] &&
             check_4_pdos[2] && check_4_pdos[3]) ||
            pdo_timeout)
        {
            // after receiving the CAN message, kill the thread
            pthread_cancel(th_timer);
            pdo_timeout = 0;
            break;
        }

        nbytes = read(s, &frame, sizeof(frame));

        if (nbytes > 0)
        {

            /**
             * @todo receive the PDO answer
             * @brief convert the 8 bytes into data
             */
            switch (frame.can_id)
            {
            case 0x1FF:
                check_4_pdos[0] = 1;
                *motor_position = frame.data[0] + (frame.data[1] << 8) + (frame.data[2] << 16) + (frame.data[3] << 24);
                break;
            case 0x2FF:
                check_4_pdos[1] = 1;
                break;
            case 0x3FF:
                check_4_pdos[2] = 1;
                break;
            case 0x4FF:
                check_4_pdos[3] = 1;
                *load_cell_voltage = frame.data[0] + (frame.data[1] << 8);

                break;
            default:
                break;
            }
        }
    }
}

void get_and_record_Pdo_response(uint32_t *motor_current,
                                 uint32_t *motor_position,
                                 uint32_t *load_cell_voltage,
                                 int data_recording_active,
                                 FILE *log_file_p)
{
    int check_4_pdos[4] = {0, 0, 0, 0};
    int nbytes;

    pthread_create(&th_timer, NULL, pdo_time_watcher, NULL);
    while (1)
    {

        /*check if the four PDOs are all updated*/
        if ((check_4_pdos[0] && check_4_pdos[1] &&
             check_4_pdos[2] && check_4_pdos[3]) ||
            pdo_timeout)
        {
            // after receiving the CAN message, kill the thread
            pthread_cancel(th_timer);
            pdo_timeout = 0;
            break;
        }

        nbytes = read(s, &frame, sizeof(frame));

        if (nbytes > 0)
        {

            /**
             * @todo receive the PDO answer
             * @brief convert the 8 bytes into data
             */
            switch (frame.can_id)
            {
            case 0x1FF:
                check_4_pdos[0] = 1;
                *motor_position = frame.data[0] + (frame.data[1] << 8) + (frame.data[2] << 16) + (frame.data[3] << 24);
                break;
            case 0x2FF:
                check_4_pdos[1] = 1;
                break;
            case 0x3FF:
                check_4_pdos[2] = 1;
                break;
            case 0x4FF:
                check_4_pdos[3] = 1;
                *load_cell_voltage = frame.data[0] + (frame.data[1] << 8);

                break;
            default:
                break;
            }
            /* if the data recording switch is ON*/
            /* record the pdo data */
            if (data_recording_active)
            {

                gettimeofday(&tv, NULL);
                strftime(buf, 32, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));

                sprintf(long_buf, "%s.%06d,%f,%X,%d,",
                        buf, tv.tv_usec,
                        (double)(tv.tv_sec - tv_start.tv_sec) * 1000 + (tv.tv_usec - tv_start.tv_usec) * 0.001,
                        frame.can_id, frame.can_dlc);
                // memset(buf,'\0',32);
                for (int i = 0; i < frame.can_dlc; i++)
                {
                    sprintf(buf, " %02X", frame.data[i]);
                    strcat(long_buf, buf);
                }
                strcat(long_buf, "\n");
                fprintf(log_file_p, long_buf);
            }
        }
    }
}