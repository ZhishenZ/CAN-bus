/**
 * @file three_boards.c
 * @author Zhishen ZHANG (zhishen.zhang@knorr-bremse.com)
 * @brief This program collects the data from three mcc118 DAQHATs synchronously.
 *        The collected data is stored in three separated csv file.
 *        The bottom board sends the CLK signal to the other mcc 118 boards,
 *        which collect sensor data at the frequency of the master board.
 *
 *        Before running this program, read the EEPROMs with the MCC DAQ HAT Manager
 *        and connect the CLK terminals on the MCC 118 boards together.
 *
 *        No jumper:     address 0
 *        Jumper on A0:  address 1
 *        Jumper on A1:  address 2
 *
 * @version 0.1
 * @date 2022-05-19
 * @copyright Copyright (c) 2022
 */

#include "daqhats_utils.h"
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#define DEVICE_COUNT 3          // here we set three devices.
#define MASTER 0                // set the master's address to be 0, which is the board at the bottom layer with out jumper.
#define SAMPLES_PER_CHANNEL 200 // the number of samples per channel
#define SAMPLES_RATE 1000       // the sample rate [Hz]
#define NUM_CHANNELS 8          // the number of channels of a board
#define LOGGING_TIME 10         // time to record the data in seconds

// selected channel of device/board 0
#define SEL_CHAN_DEV_0 CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5 | CHAN6 | CHAN7
// selected channel of device/board 1
#define SEL_CHAN_DEV_1 CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5 | CHAN6 | CHAN7
// selected channel of device/board 2
#define SEL_CHAN_DEV_2 CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5 | CHAN6 | CHAN7

int get_hat_addresses(uint8_t address[]);

int scan_init(uint8_t address[], int chan_count[], uint32_t options[],
              uint8_t chan_mask[], int chans[][NUM_CHANNELS], char options_str[][256],
              double sample_rate, double actual_sample_rate, char chan_display[][32]);

int creat_log_files(FILE *log_file_ptr[]);

int add_log_files_header(FILE *log_file_ptr[],
                         char chan_name[][8][32], uint8_t chan_mask[]);

void initial_print(double sample_rate, double actual_sample_rate, uint8_t address[],
                   char chan_display[][32], char options_str[][256]);

int write_log_file(FILE *log_file_ptr[], double elapsed_time[],
                   double read_buf[][SAMPLES_PER_CHANNEL * NUM_CHANNELS * 5],
                   uint32_t samples_read[], const double dt);

void display_data_in_terminal(uint32_t samples_read[], int chan_count[],
                              int total_samples_read[], int chans[][NUM_CHANNELS],
                              int sample_idx, double data[][sample_idx]);

int check_overrun(uint16_t scan_status_all);

void clean_up_and_stop(uint8_t address[]);

void close_log_files(FILE *log_file_ptr[]);

// FILE* open_log_file (char* path, FILE* log_file_ptr);

int main(void)
{

    uint16_t is_running = 0;
    uint16_t scan_status_all = 0;

    int i = 0;
    int device = 0;
    int sample_idx = 0;
    int result = 0;
    int chan_count[DEVICE_COUNT];
    int chans[DEVICE_COUNT][NUM_CHANNELS];
    int total_samples_read[DEVICE_COUNT] = {0};
    int buffer_size = SAMPLES_PER_CHANNEL * NUM_CHANNELS * 5; // make sure that there is enough space for the buffer.
    // const int data_display_line_count = DEVICE_COUNT * 4;
    double elapsed_time[DEVICE_COUNT] = {0};
    double dt = 1.0 / SAMPLES_RATE * 1000; // convert into miliseconds
    double sample_rate = SAMPLES_RATE;     // Samples per second
    double actual_sample_rate = 0.0;

    struct timeval this_time, last_time; // time stamp
    FILE *log_file_ptr[DEVICE_COUNT] = {NULL};

    // HAT device addresses - determined at runtime
    uint8_t address[DEVICE_COUNT];
    // all channels open
    uint8_t chan_mask[DEVICE_COUNT] = {SEL_CHAN_DEV_0, SEL_CHAN_DEV_1, SEL_CHAN_DEV_2};

    char chan_display[DEVICE_COUNT][32];
    char options_str[DEVICE_COUNT][256];

    // mcc118_a_in_scan_start() variables
    char chan_name[DEVICE_COUNT][NUM_CHANNELS][32] =
        {{"Chan0_Dev0", "Chan1_Dev0", "Chan2_Dev0", "Chan3_Dev0", "Chan4_Dev0", "Chan5_Dev0", "Chan6_Dev0", "Chan7_Dev0"},
         {"Chan0_Dev1", "Chan1_Dev1", "Chan2_Dev1", "Chan3_Dev1", "Chan4_Dev1", "Chan5_Dev1", "Chan6_Dev1", "Chan7_Dev1"},
         {"Chan0_Dev2", "Chan1_Dev2", "Chan2_Dev2", "Chan3_Dev2", "Chan4_Dev2", "Chan5_Dev2", "Chan6_Dev2", "Chan7_Dev2"}};

    uint32_t options[DEVICE_COUNT] = {
        OPTS_CONTINUOUS,                  // for device 0
        OPTS_CONTINUOUS | OPTS_EXTCLOCK,  // for device 1
        OPTS_CONTINUOUS | OPTS_EXTCLOCK}; // for device 2

    uint16_t scan_status[DEVICE_COUNT] = {0};
    uint32_t samples_read[DEVICE_COUNT] = {0};

    // Determine the addresses of the devices to be used
    if (get_hat_addresses(address) != 0)
    {
        return -1;
    }

    result = scan_init(address, chan_count, options, chan_mask, chans, options_str,
                       sample_rate, actual_sample_rate, chan_display);
    STOP_ON_ERROR(result);

    initial_print(sample_rate, actual_sample_rate, address,
                  chan_display, options_str);

    // Start the scan
    for (device = 0; device < DEVICE_COUNT; device++)
    {
        result = mcc118_a_in_scan_start(address[device], chan_mask[device],
                                        0, sample_rate, options[device]);
        STOP_ON_ERROR(result);
    }

    // time stamp of the scanning starting time
    gettimeofday(&last_time, NULL);

    result = mcc118_a_in_scan_status(address[MASTER], &scan_status[MASTER],
                                     NULL);
    STOP_ON_ERROR(result);
    is_running = (scan_status[MASTER] & STATUS_RUNNING);

    if (is_running)
    {
        printf("Acquiring data ... Press 'Enter' to abort\n\n");
    }
    else
    {
        printf("Aborted\n\n");
        goto stop;
    }

    if (!creat_log_files(log_file_ptr))
        goto stop;

    // add headers
    if (!add_log_files_header(log_file_ptr, chan_name, chan_mask))
        goto stop;

    while (is_running)
    {
        usleep(200000); // sleep for 200000 microseconds/200 ms /0.2 s

        double data[DEVICE_COUNT][buffer_size];

        // read the data
        for (i = 0; i < DEVICE_COUNT; i++)
        {
            result = mcc118_a_in_scan_read(address[i], &scan_status[i], READ_ALL_AVAILABLE,
                                           0, data[i], buffer_size, &samples_read[i]);
            STOP_ON_ERROR(result);
            // Check for overrun on any one device
            scan_status_all |= scan_status[i];
            is_running &= (scan_status[i] & STATUS_RUNNING);
        }

        if (!check_overrun(scan_status_all))
            goto stop;

        if (!write_log_file(log_file_ptr, elapsed_time, data, &samples_read[0], dt))
        {
            fprintf(stderr, "\nWrite log file failed.\n");
            break;
        };

        // if the logging time is reached,
        // close the previous file and open a new file
        gettimeofday(&this_time, NULL);

        if (this_time.tv_sec - last_time.tv_sec >= LOGGING_TIME)
        {
            printf("\n-----------------------------"
                   "\nMaximal logging time reached.\nOld files were closed.\n");

            // close the files and creat a new file and add the header to the files
            for (i = 0; i < DEVICE_COUNT; i++)
            {
                fprintf(log_file_ptr[i], "\nFINISHED");
                fclose(log_file_ptr[i]);

                // set the sample number to zero
                total_samples_read[i] = 0;
            }

            if (!creat_log_files(log_file_ptr))
                goto stop;

            printf("New files created.\n-----------------------------\n\n");

            if (!add_log_files_header(log_file_ptr, chan_name, chan_mask))
                goto stop;
            // update last_time
            last_time = this_time;
        }
        // elapsed_time+=scanning_period;
        display_data_in_terminal(samples_read, chan_count, total_samples_read, chans, sample_idx, data);

        if (enter_press())
        {
            printf("Acquiring data stopped. \n\n");
            break;
        }
    }

stop:
    // Stop and cleanup
    clean_up_and_stop(address);
    printf("All devices are stopped.\n");

    // CLose the log files
    close_log_files(log_file_ptr);
    printf("All log files closed.\n");

    return 0;
}

/* This function gets the addresses of the MCC 118 devices to be used. */
int get_hat_addresses(uint8_t address[])
{
    int return_val = -1;
    int hat_count = 0;
    int device = 0;
    int i = 0;
    int address_int;

    int valid = 0;

    hat_count = hat_list(HAT_ID_MCC_118, NULL);

    if (hat_count >= DEVICE_COUNT)
    {
        struct HatInfo *hats = (struct HatInfo *)malloc(
            hat_count * sizeof(struct HatInfo));
        hat_list(HAT_ID_MCC_118, hats);

        if (hat_count == DEVICE_COUNT)
        {
            for (device = 0; device < DEVICE_COUNT; device++)
            {
                address[device] = hats[device].address;
                // printf("address[device]%d",address[device]);
            }
        }
        else
        {
            for (i = 0; i < hat_count; i++)
            {
                printf("Address %d: %s\n", hats[i].address,
                       hats[i].product_name);
            }

            for (device = 0; device < DEVICE_COUNT; device++)
            {
                valid = 0;
                while (!valid)
                {
                    printf("\n Enter address for HAT device %d: ", device);
                    scanf("%d", &address_int);

                    // Check if the address exists in the hats array
                    for (i = 0; i < hat_count; i++)
                    {
                        if (hats[i].address == address_int)
                            valid = 1;
                    }
                    // Make sure the address was not previously selected
                    for (i = 0; i < device; i++)
                    {
                        if (address[i] == address_int)
                            valid = 0;
                    }

                    if (valid)
                    {
                        address[device] = (uint8_t)address_int;
                    }
                    else
                    {
                        printf("Invalid address - try again");
                    }
                }
            }
            flush_stdin();
        }
        free(hats);
        return_val = 0;
    }
    else
    {
        fprintf(stderr, "Error: This example requires %d MCC 118 devices - "
                        "found %d\n",
                DEVICE_COUNT, hat_count);
    }

    return return_val;
}

// Convert the numeric data to ASCII values, seperated by commas (CSV), and
// write the data using the specified file pointer.
int write_log_file(FILE *log_file_ptr[], double elapsed_time[],
                   double read_buf[][SAMPLES_PER_CHANNEL * NUM_CHANNELS * 5],
                   uint32_t samples_read[], const double dt)
{
    int write_status = 0;
    char str[1000];
    char buf[32];
    char daytime[50];

    struct timeval tv;
    gettimeofday(&tv, NULL);
    int micro = tv.tv_usec;
    strftime(buf, 32, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec));

    sprintf(daytime, "%s.%06d,", buf, micro);

    for (int dev = 0; dev < DEVICE_COUNT; dev++)
    {
        for (int i = 0; i < samples_read[dev]; i++)
        {
            strcpy(str, ""); // set the str empty
            strcat(str, daytime);

            sprintf(buf, "%2.6f,", elapsed_time[dev]);
            strcat(str, buf);
            elapsed_time[dev] += dt;
            // Write a sample for each channel in the scan.
            for (int j = 0; j < NUM_CHANNELS; j++)
            {
                // Convert the data sample to ASCII
                sprintf(buf, "%2.6f,", read_buf[dev][NUM_CHANNELS * i + j]);
                // Add the data sample to the string to be written.
                strcat(str, buf);
            }

            str[strlen(str) - 1] = '\0'; // delete the last comma

            write_status = fprintf(log_file_ptr[dev], "%s\n", str);
            if (write_status <= 0)
            {
                fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
                return write_status;
            }
        }
        fflush(log_file_ptr[dev]);
    }

    return write_status;
}

void display_data_in_terminal(uint32_t samples_read[], int chan_count[],
                              int total_samples_read[], int chans[][NUM_CHANNELS],
                              int sample_idx, double data[][sample_idx])
{
    int i = 0;
    char display_string[256] = "";
    char data_display_output[1024] = "";

    for (int device = 0; device < DEVICE_COUNT; device++)
    {
        strcpy(data_display_output, "");

        sprintf(display_string, "HAT %d:    samples_read:%d\n", device, samples_read[device]);
        strcat(data_display_output, display_string);

        // Display the header row for the data table
        strcat(data_display_output, "  Samples Read    Scan Count");
        for (i = 0; i < chan_count[device]; i++)
        {
            sprintf(display_string, "     Channel %d", chans[device][i]);
            strcat(data_display_output, display_string);
        }
        strcat(data_display_output, "\n");

        // Display the sample count information for the device
        total_samples_read[device] += samples_read[device];
        sprintf(display_string, "%14d%14d", samples_read[device],
                total_samples_read[device]);
        strcat(data_display_output, display_string);

        // Display the data for all active channels
        if (samples_read[device] > 0)
        {
            for (i = 0; i < chan_count[device]; i++)
            {
                sample_idx = (samples_read[device] * chan_count[device]) -
                             chan_count[device] + i;
                sprintf(display_string, "%12.5f V",
                        data[device][sample_idx]);
                strcat(data_display_output, display_string);
            }
        }

        strcat(data_display_output, "\n\n");
        printf(data_display_output);
    }

    fflush(stdout);
}

void clean_up_and_stop(uint8_t address[])
{

    int result;

    // Stop and cleanup
    for (int device = 0; device < DEVICE_COUNT; device++)
    {
        result = mcc118_a_in_scan_stop(address[device]);
        print_error(result);
        result = mcc118_a_in_scan_cleanup(address[device]);
        print_error(result);
        result = mcc118_close(address[device]);
        print_error(result);
    }
}

void initial_print(double sample_rate, double actual_sample_rate, uint8_t address[],
                   char chan_display[][32], char options_str[][256])
{
    char c;
    printf("    Samples per channel: %d\n", SAMPLES_PER_CHANNEL);
    printf("    Requested Sample Rate: %.3f Hz\n", sample_rate);
    printf("    Actual Sample Rate: %.3f Hz\n", actual_sample_rate);

    for (int device = 0; device < DEVICE_COUNT; device++)
    {
        printf("    MCC 118 %d:\n", device);
        printf("      Address: %d\n", address[device]);
        printf("      Channels: %s\n", chan_display[device]);
        printf("      Options: %s\n", options_str[device]);
    }
    printf("\n*NOTE: Connect the CLK terminals together on each MCC 118 device "
           "being used.");
    printf("\nPress 'Enter' to continue\n");
    scanf("%c", &c);
}

int check_overrun(uint16_t scan_status_all)
{

    if ((scan_status_all & STATUS_HW_OVERRUN) == STATUS_HW_OVERRUN)
    {
        fprintf(stderr, "\nError: Hardware overrun\n");
        return 0;
    }
    if ((scan_status_all & STATUS_BUFFER_OVERRUN) == STATUS_BUFFER_OVERRUN)
    {
        fprintf(stderr, "\nError: Buffer overrun\n");
        return 0;
    }

    return 1;
}

int scan_init(uint8_t address[], int chan_count[], uint32_t options[],
              uint8_t chan_mask[], int chans[][NUM_CHANNELS], char options_str[][256],
              double sample_rate, double actual_sample_rate, char chan_display[][32])
{
    int result;
    for (int device = 0; device < DEVICE_COUNT; device++)
    {
        // Open a connection to each device
        result = mcc118_open(address[device]);
        // STOP_ON_ERROR(result0);
        if (result)
            return result;
        // channel_mask is used by the library function mcc118_a_in_scan_start.
        // The functions below parse the mask for display purposes.
        chan_count[device] = convert_chan_mask_to_array(chan_mask[device], chans[device]);
        convert_chan_mask_to_string(chan_mask[device], chan_display[device]);
        convert_options_to_string(options[device], options_str[device]);
    }

    result = mcc118_a_in_scan_actual_rate(chan_count[MASTER], sample_rate,
                                          &actual_sample_rate);
    return result;
}

int creat_log_files(FILE *log_file_ptr[DEVICE_COUNT])
{

    // set the pathLogFiles
    char path_LogFiles[512];
    struct tm *current_tm;
    time_t this_time_t;

    char time_string[32];
    char log_filename[DEVICE_COUNT][512];

    getcwd(path_LogFiles, sizeof(path_LogFiles));
    strcat(path_LogFiles, "/LogFiles/");

    // Initialize the output file
    this_time_t = time(NULL);
    current_tm = localtime(&this_time_t);
    // csv file name
    sprintf(time_string, "%02d_%02d_%02d_%02d_%02d_%02d_%s", current_tm->tm_year + 1900,
            (current_tm->tm_mon + 1), current_tm->tm_mday, current_tm->tm_hour,
            current_tm->tm_min, current_tm->tm_sec, current_tm->tm_zone);

    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        strcpy(log_filename[i], "");
        sprintf(log_filename[i], "%s%s_DevAddr%d.csv", path_LogFiles, time_string, i);
        log_file_ptr[i] = fopen(log_filename[i], "w");
        if (!log_file_ptr[i])
        {
            printf("Open file errno = %d reason %s \n", errno, strerror(errno));
            printf("Please create a directory called \"LogFiles\" "
                   "in the directory \"/three_boards.\" \n\n");

            return 0;
        }
    }

    return 1;
}

int add_log_files_header(FILE *log_file_ptr[], char chan_name[][8][32],
                         uint8_t chan_mask[])
{
    char str[256];
    int write_status;
    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        strcpy(str, "");
        strcat(str, "Day Time,Time(ms),");
        for (int j = 0; j < NUM_CHANNELS; j++)
        {
            if (chan_mask[i] & (0x01 << j))
            {
                strcat(str, chan_name[i][j]);
                strcat(str, ",");
            }
        }

        str[strlen(str) - 1] = '\0';
        write_status = fprintf(log_file_ptr[i], "%s\n", str);
        if (write_status <= 0)
        {
            fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
            return 0;
        }
        fflush(log_file_ptr[i]);
    }

    return 1;
};

void close_log_files(FILE *log_file_ptr[DEVICE_COUNT])
{

    for (int i = 0; i < DEVICE_COUNT; i++)
    {
        fclose(log_file_ptr[i]);
    }
}