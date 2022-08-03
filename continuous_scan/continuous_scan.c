#include "daqhats_utils.h"
#include "test.h"
#include <time.h>
#include <errno.h>
#include <sys/time.h>

// logging time for each file in seconds
#define LOGGING_TIME 10 // s

void print_data(uint32_t samples_read_per_channel, uint8_t num_channels, double read_buf[]);

int check_overrun(uint16_t read_status);

int create_log_file(FILE **log_file_ptr, uint8_t channel_mask, char channel_name[][32], uint8_t num_channels);

int write_log_file(FILE *log_file_ptr, uint8_t num_channels, double *read_buf,
                   uint32_t *samples_read, double *elapsed_time, const double dt);
int main(void)
{
    int result = RESULT_SUCCESS;
    uint8_t address = 0;
    char c;
    char display_header[512];
    int i;
    char channel_string[512];
    char options_str[512];

    // Set the channel mask which is used by the library function
    // mcc118_a_in_scan_start to specify the channels to acquire.
    // The functions below, will parse the channel mask into a
    // character string for display purposes.
    uint8_t channel_mask = {CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5 | CHAN6 | CHAN7};
    char channel_name[8][32] =
        {"Chan0_Dev0", "Chan1_Dev0", "Chan2_Dev0", "Chan3_Dev0", "Chan4_Dev0", "Chan5_Dev0", "Chan6_Dev0", "Chan7_Dev0"};

    convert_chan_mask_to_string(channel_mask, channel_string);

    uint32_t samples_per_channel = 0;

    int max_channel_array_length = mcc118_info()->NUM_AI_CHANNELS;
    int channel_array[max_channel_array_length];
    uint8_t num_channels = convert_chan_mask_to_array(channel_mask,
                                                      channel_array);

    uint32_t internal_buffer_size_samples = 0;
    uint32_t user_buffer_size = 1000 * num_channels;
    double read_buf[user_buffer_size];
    int32_t read_request_size = READ_ALL_AVAILABLE;

    // When doing a continuous scan, the timeout value will be ignored in the
    // call to mcc118_a_in_scan_read because we will be requesting that all
    // available samples (up to the default buffer size) be returned.
    double timeout = 5.0;

    double scan_rate = 1000.0;
    double actual_scan_rate = 0.0;
    mcc118_a_in_scan_actual_rate(num_channels, scan_rate, &actual_scan_rate);

    uint32_t options = OPTS_CONTINUOUS;

    uint16_t read_status = 0;
    uint32_t samples_read_per_channel = 0;

    struct timeval this_time, last_time;

    // Select an MCC118 HAT device to use.
    if (select_hat_device(HAT_ID_MCC_118, &address))
    {
        // Error getting device.
        return -1;
    }

    printf("\nSelected MCC 118 device at address %d\n", address);

    // Open a connection to the device.
    result = mcc118_open(address);
    STOP_ON_ERROR(result);

    convert_options_to_string(options, options_str);
    convert_chan_mask_to_string(channel_mask, channel_string);

    printf("\nMCC 118 continuous scan example\n");
    printf("    Functions demonstrated:\n");
    printf("        mcc118_a_in_scan_start\n");
    printf("        mcc118_a_in_scan_read\n");
    printf("        mcc118_a_in_scan_stop\n");
    printf("    Channels: %s\n", channel_string);
    printf("    Requested scan rate: %-10.2f\n", scan_rate);
    printf("    Actual scan rate: %-10.2f\n", actual_scan_rate);
    printf("    Options: %s\n", options_str);

    printf("\nPress ENTER to continue ...\n");
    scanf("%c", &c);

    // Configure and start the scan.
    // Since the continuous option is being used, the samples_per_channel
    // parameter is ignored if the value is less than the default internal
    // buffer size (10000 * num_channels in this case). If a larger internal
    // buffer size is desired, set the value of this parameter accordingly.
    result = mcc118_a_in_scan_start(address, channel_mask, samples_per_channel,
                                    scan_rate, options);
    STOP_ON_ERROR(result);

    gettimeofday(&last_time, NULL);

    STOP_ON_ERROR(mcc118_a_in_scan_buffer_size(address,
                                               &internal_buffer_size_samples));
    printf("Internal data buffer size:  %d\n", internal_buffer_size_samples);

    printf("\nStarting scan ... Press ENTER to stop\n\n");

    // Create the header containing the column names.
    strcpy(display_header, "Samples Read    ");
    for (i = 0; i < num_channels; i++)
    {
        sprintf(channel_string, "Channel %d   ", channel_array[i]);
        strcat(display_header, channel_string);
    }
    strcat(display_header, "\n");
    printf("%s", display_header);

    // add the file pointer
    FILE *log_file_ptr = NULL;

    const double dt = 1.0 / scan_rate * 1000; // convert into miliseconds
    double elapsed_time = 0;

    // create a
    if (!create_log_file(&log_file_ptr, channel_mask, channel_name, num_channels))
        goto stop;

    // Continuously update the display value until enter key is pressed
    do
    {

        usleep(200000);
        // Since the read_request_size is set to -1 (READ_ALL_AVAILABLE), this
        // function returns immediately with whatever samples are available (up
        // to user_buffer_size) and the timeout parameter is ignored.
        result = mcc118_a_in_scan_read(address, &read_status, read_request_size,
                                       timeout, read_buf, user_buffer_size, &samples_read_per_channel);
        STOP_ON_ERROR(result);

        if (!check_overrun(read_status))
            break;

        if (!write_log_file(log_file_ptr, num_channels, read_buf,
                            &samples_read_per_channel, &elapsed_time, dt))
        {
            fprintf(stderr, "\nWrite log file failed.\n");
            break;
        };

        gettimeofday(&this_time, NULL);

        if (this_time.tv_sec - last_time.tv_sec >= LOGGING_TIME)
        {

            fclose(log_file_ptr);
            printf("\n-----------------------------"
                   "\nMaximal logging time reached.\nOld files were closed.\n");
            // close the files and creat a new file and add the header to the files

            if (!create_log_file(&log_file_ptr, channel_mask, channel_name, num_channels))
                goto stop;

            printf("New files created.\n-----------------------------\n\n");

            // update last_time
            last_time = this_time;
        }

        print_data(samples_read_per_channel, num_channels, read_buf);

        // order a file

    } while ((result == RESULT_SUCCESS) &&
             ((read_status & STATUS_RUNNING) == STATUS_RUNNING) &&
             !enter_press());

    printf("\n\r");

stop:
    print_error(mcc118_a_in_scan_stop(address));
    print_error(mcc118_a_in_scan_cleanup(address));
    print_error(mcc118_close(address));

    return 0;
}

void print_data(uint32_t samples_read_per_channel, uint8_t num_channels, double read_buf[])
{

    printf("\r%12.0d ", samples_read_per_channel);

    if (samples_read_per_channel > 0)
    {
        int index = samples_read_per_channel * num_channels - num_channels;

        for (int i = 0; i < num_channels; i++)
        {
            printf("%10.5f V", read_buf[index + i]);
        }
        fflush(stdout);
    }
}

int check_overrun(uint16_t read_status)
{

    if (read_status & STATUS_HW_OVERRUN)
    {
        printf("\n\nHardware overrun\n");
        return 0;
    }
    else if (read_status & STATUS_BUFFER_OVERRUN)
    {
        printf("\n\nBuffer overrun\n");
        return 0;
    }

    return 1;
}

int create_log_file(FILE **log_file_ptr, uint8_t channel_mask, char channel_name[][32], uint8_t num_channels)
{

    // set the pathLogFiles
    char path_LogFiles[128];
    struct tm *current_tm;
    time_t this_time_t;

    char time_string[32];
    char log_filename_device[512] = "";

    getcwd(path_LogFiles, sizeof(path_LogFiles));
    strcat(path_LogFiles, "/LogFiles/");

    // Initialize the output file
    this_time_t = time(NULL);
    current_tm = localtime(&this_time_t);
    // csv file name
    sprintf(time_string, "%02d_%02d_%02d_%02d_%02d_%02d_%s.csv", current_tm->tm_year + 1900,
            (current_tm->tm_mon + 1), current_tm->tm_mday, current_tm->tm_hour,
            current_tm->tm_min, current_tm->tm_sec, current_tm->tm_zone);

    strcat(log_filename_device, path_LogFiles);
    strcat(log_filename_device, time_string);
    // strcat(log_filename_device,"_DevAddr0.csv");

    // Open the log file
    *log_file_ptr = fopen(log_filename_device, "w");

    if (*log_file_ptr == NULL)
    {
        printf("Open file errno = %d reason %s \n", errno, strerror(errno));
        printf("Please create a directory called \"LogFiles\" "
               "in the current directory \n\n");

        return 0;
    }

    char str_0[256] = "Day Time,Time(ms),";
    for (int i = 0; i < num_channels; i++)
    {
        if (channel_mask & (0x01 << i))
        {
            strcat(str_0, channel_name[i]);
            strcat(str_0, ",");
            // printf("chane_name: %s\n",chan_name[0][i]);
        }
    }
    str_0[strlen(str_0) - 1] = '\0';

    int write_status = fprintf(*log_file_ptr, "%s\n", str_0);
    if (write_status <= 0)
    {
        fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
        return 0;
        // break;
    }
    // Flush the file to ensure all data is written.
    fflush(*log_file_ptr);

    return 1;
}

int write_log_file(FILE *log_file_ptr, uint8_t num_channels,
                   double *read_buf, uint32_t *samples_read /*get from scan function*/,
                   double *elapsed_time, const double dt)
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
    strcpy(str, "");

    for (int i = 0; i < *samples_read; i++)
    {
        strcpy(str, "");
        strcat(str, daytime);

        sprintf(buf, "%2.6f,", *elapsed_time);
        strcat(str, buf);
        *elapsed_time += dt;
        // Write a sample for each channel in the scan.
        for (int j = 0; j < num_channels; j++)
        {
            // Convert the data sample to ASCII
            sprintf(buf, "%2.6f,", read_buf[num_channels * i + j]);
            // Add the data sample to the string to be written.
            strcat(str, buf);
        }

        str[strlen(str) - 1] = '\0';

        write_status = fprintf(log_file_ptr, "%s\n", str);
        if (write_status <= 0)
        {
            fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
            break;
        }
    }

    // Flush the file to ensure all data is written.
    fflush(log_file_ptr);

    return write_status;
}