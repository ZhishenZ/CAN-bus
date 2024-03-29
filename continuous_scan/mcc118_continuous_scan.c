#include "mcc118_continuous_scan.h"

void print_buffer_info(uint8_t address)
{

    uint32_t internal_buffer_size_samples = 0;

    STOP_ON_ERROR(mcc118_a_in_scan_buffer_size(address, &internal_buffer_size_samples), address);

    printf("Internal data buffer size:  %d\n", internal_buffer_size_samples);

    printf("\nStarting scan ... Press ENTER to stop\n\n");
};

void print_header_info(uint8_t num_channels,
                       char channel_string[512], int *channel_array)
{
    // Create the header containing the column names.
    char display_header[512];
    strcpy(display_header, "Samples Read    ");
    for (int i = 0; i < num_channels; i++)
    {
        sprintf(channel_string, "Channel %d   ", channel_array[i]);
        strcat(display_header, channel_string);
    }
    strcat(display_header, "\n");
    printf("%s", display_header);
}

void stop_and_cleanup(uint8_t address)
{
    print_error(mcc118_a_in_scan_stop(address));
    print_error(mcc118_a_in_scan_cleanup(address));
    print_error(mcc118_close(address));
}

int mcc118_init_print(uint8_t num_channels, double scan_rate, uint8_t *address,
                      uint32_t options, uint8_t channel_mask, char channel_string[512])
{

    double actual_scan_rate = 0.0;
    char options_str[512];
    char c;

    mcc118_a_in_scan_actual_rate(num_channels, scan_rate, &actual_scan_rate);

    // Select an MCC118 HAT device to use.
    if (select_hat_device(HAT_ID_MCC_118, address))
    {
        // Error getting device.
        return 0;
    }

    printf("\nSelected MCC 118 device at address %d\n", *address);

    if (mcc118_open(*address) != RESULT_SUCCESS)
    {
        stop_and_cleanup(*address);
        return 0;
    };

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

/****************************************************************************
 * Conversion functions
 ****************************************************************************/

/* This function converts the mask of options defined by the options parameter
   and sets the options_str parameter, which is passed by reference, to a
   comma separated string respresentation of the options. */
void convert_options_to_string(uint32_t options, char *options_str)
{
    if (options_str == NULL)
        return;

    strcpy(options_str, "");

    if (options == OPTS_DEFAULT)
    {
        strcat(options_str, "OPTS_DEFAULT");
    }
    else
    {
        if (options & OPTS_NOSCALEDATA)
        {
            strcat(options_str, "OPTS_NOSCALEDATA, ");
        }
        if (options & OPTS_NOCALIBRATEDATA)
        {
            strcat(options_str, "OPTS_NOCALIBRATEDATA, ");
        }
        if (options & OPTS_EXTCLOCK)
        {
            strcat(options_str, "OPTS_EXTCLOCK, ");
        }
        if (options & OPTS_EXTTRIGGER)
        {
            strcat(options_str, "OPTS_EXTTRIGGER, ");
        }
        if (options & OPTS_CONTINUOUS)
        {
            strcat(options_str, "OPTS_CONTINUOUS, ");
        }
        *strrchr(options_str, ',') = '\0';
    }
}

/* This function converts the trigger mode defined by the trigger_mode
   parameter to a string representation and returns the string
   respresentation of the trigger mode. */
void convert_trigger_mode_to_string(uint8_t trigger_mode,
                                    char *trigger_mode_str)
{
    if (trigger_mode_str == NULL)
        return;

    switch (trigger_mode)
    {
    case TRIG_FALLING_EDGE:
        strcpy(trigger_mode_str, "TRIG_FALLING_EDGE");
        break;
    case TRIG_ACTIVE_HIGH:
        strcpy(trigger_mode_str, "TRIG_ACTIVE_HIGH");
        break;
    case TRIG_ACTIVE_LOW:
        strcpy(trigger_mode_str, "TRIG_ACTIVE_LOW");
        break;
    case TRIG_RISING_EDGE:
    default:
        strcpy(trigger_mode_str, "TRIG_RISING_EDGE");
        break;
    }
    return;
}

/* This function converts the analog input mode defined by the mode
   parameter to a string representation and returns the string
   respresentation of the input mode. */
void convert_input_mode_to_string(uint8_t mode, char *mode_str)
{
    if (mode_str == NULL)
        return;

    switch (mode)
    {
    case A_IN_MODE_SE:
        strcpy(mode_str, "SINGLE_ENDED");
        break;
    case A_IN_MODE_DIFF:
    default:
        strcpy(mode_str, "DIFFERENTIAL");
        break;
    }
    return;
}

/* This function converts the analog input range defined by the range
   parameter to a string representation and returns the string
   respresentation of the input range. */
void convert_input_range_to_string(uint8_t range, char *range_str)
{
    if (range_str == NULL)
        return;

    switch (range)
    {
    case A_IN_RANGE_BIP_10V:
        strcpy(range_str, "+/- 10 V");
        break;
    case A_IN_RANGE_BIP_5V:
        strcpy(range_str, "+/- 5 V");
        break;
    case A_IN_RANGE_BIP_2V:
        strcpy(range_str, "+/- 2 V");
        break;
    case A_IN_RANGE_BIP_1V:
    default:
        strcpy(range_str, "+/- 1 V");
        break;
    }
    return;
}

/* This function converts the thermocouple type defined by the tc_type
   parameter to a string representation and returns the string
   respresentation. */
void convert_tc_type_to_string(uint8_t tc_type,
                               char *tc_type_str)
{
    if (tc_type_str == NULL)
        return;

    switch (tc_type)
    {
    case TC_TYPE_J:
        strcpy(tc_type_str, "J");
        break;
    case TC_TYPE_K:
        strcpy(tc_type_str, "K");
        break;
    case TC_TYPE_T:
        strcpy(tc_type_str, "T");
        break;
    case TC_TYPE_E:
        strcpy(tc_type_str, "E");
        break;
    case TC_TYPE_R:
        strcpy(tc_type_str, "R");
        break;
    case TC_TYPE_S:
        strcpy(tc_type_str, "S");
        break;
    case TC_TYPE_B:
        strcpy(tc_type_str, "B");
        break;
    case TC_TYPE_N:
        strcpy(tc_type_str, "N");
        break;
    case TC_DISABLED:
    default:
        strcpy(tc_type_str, "DISABLED");
        break;
    }
    return;
}

/* This function converts the mask of channels defined by the channel_mask
   parameter and sets the chans_str parameter, which is passed by reference,
   to a comma separated string respresentation of the channel numbers. */
void convert_chan_mask_to_string(uint32_t channel_mask, char *chans_str)
{
    int i = 0;
    char chan_string[8];

    if (chans_str == NULL)
        return;

    strcpy(chans_str, "");

    while (channel_mask > 0)
    {
        if (channel_mask & 0x01)
        {
            sprintf(chan_string, "%d, ", i);
            strcat(chans_str, chan_string);
        }
        i++;
        channel_mask >>= 1;
    }
    *strrchr(chans_str, ',') = '\0';
}

/* This function converts the mask of channels defined by the channel_mask
   parameter and sets the chans parameter, which is passed by reference,
   to an array of channel numbers.
   The return value is an integer representing the number of channels. */
int convert_chan_mask_to_array(uint32_t channel_mask, int chans[])
{
    int i = 0;
    int chan_count = 0;

    if (chans == NULL)
        return 0;

    while (channel_mask > 0)
    {
        if (channel_mask & 0x01)
        {
            chans[chan_count] = i;
            chan_count++;
        }
        i++;
        channel_mask >>= 1;
    }

    return chan_count;
}

/****************************************************************************
 * Display functions
 ****************************************************************************/
/* This function takes a result code as the result parameter and if the
   result code is not RESULT_SUCCESS, the error message is sent to stderr. */
void print_error(int result)
{
    if (result != RESULT_SUCCESS)
    {
        fprintf(stderr, "\nError: %s\n", hat_error_message(result));
    }
}

void resetCursor() { printf("\033[1;1H"); }
void clearEOL() { printf("\033[2K"); }
void cursorUp() { printf("\033[A"); }

/****************************************************************************
 * User input functions
 ****************************************************************************/
void flush_stdin(void)
{
    int c;

    do
    {
        c = getchar();
    } while (c != '\n' && c != EOF);
}

int enter_press()
{
    int stdin_value = 0;
    struct timeval tv;
    fd_set fds;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); // STDIN_FILENO is 0
    select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    stdin_value = FD_ISSET(STDIN_FILENO, &fds);
    if (stdin_value != 0)
    {
        flush_stdin();
    }

    return stdin_value;
}

/* This function displays the available DAQ HAT devices and allows the user
   to select a device to use with the associated example.  The address
   parameter, which is passed by reference, is set to the selected address.
   The return value is 0 for success and -1 for error.*/
int select_hat_device(uint16_t hat_filter_id, uint8_t *address)
{
    struct HatInfo *hats = NULL;
    int hat_count = 0;
    int address_int = 0;
    int return_val = -1;
    int i;

    if (address == NULL)
        return -1;

    // Get the number of HAT devices that are connected that are of the
    // requested type.
    hat_count = hat_list(hat_filter_id, NULL);

    // Verify there are HAT devices connected that are of the requested type.
    if (hat_count > 0)
    {
        // Allocate memory for the list of HAT devices.
        hats = (struct HatInfo *)malloc(hat_count * sizeof(struct HatInfo));

        // Get the list of HAT devices.
        hat_list(hat_filter_id, hats);

        if (hat_count == 1)
        {
            // Get the address of the only HAT device.
            *address = hats[0].address;
            return_val = 0;
        }
        else
        {
            // There is more than 1 HAT device so display the
            // list of devices and let the user choose one.
            for (i = 0; i < hat_count; i++)
            {
                printf("Address %d: %s\n", hats[i].address,
                       hats[i].product_name);
            }

            printf("\nSelect the address of the HAT device to use: ");
            if (scanf("%d", &address_int) == 0)
            {
                fprintf(stderr, "Error: Invalid selection\n");
                free(hats);
                return -1;
            }

            *address = (uint8_t)address_int;

            // Find the HAT device with the specified address in the list.
            for (i = 0; i < hat_count; i++)
            {
                if (hats[i].address == *address)
                {
                    return_val = 0;
                    break;
                }
            }

            if (return_val != 0)
            {
                fprintf(stderr, "Error: Invalid HAT address\n");
            }
            flush_stdin();
        }

        // Release the memory used by the HatInfo list.
        if (hats != NULL)
        {
            free(hats);
        }
    }
    else
    {
        fprintf(stderr, "Error: No HAT devices found\n");
    }

    return return_val;
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

int create_mcc118_log_file(FILE **log_file_ptr, uint8_t channel_mask, char channel_name[][32], uint8_t num_channels)
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

int mcc118_continuous_scan(void)
{
    int result = RESULT_SUCCESS;
    uint8_t address = 0;

    char channel_string[512];

    // Set the channel mask which is used by the library function
    // mcc118_a_in_scan_start to specify the channels to acquire.
    // The functions below, will parse the channel mask into a
    // character string for display purposes.
    uint8_t channel_mask = {CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5 | CHAN6 | CHAN7};
    char channel_name[8][32] =
        {"Chan0", "Chan1", "Chan2", "Chan3", "Chan4", "Chan5", "Chan6", "Chan7"};

    convert_chan_mask_to_string(channel_mask, channel_string);

    uint32_t samples_per_channel = 0;
    int max_channel_array_length = mcc118_info()->NUM_AI_CHANNELS;
    int channel_array[max_channel_array_length];
    uint8_t num_channels = convert_chan_mask_to_array(channel_mask, channel_array);
    uint16_t read_status = 0;
    uint32_t samples_read_per_channel = 0;
    uint32_t user_buffer_size = 1000 * num_channels;
    double read_buf[user_buffer_size];
    int32_t read_request_size = READ_ALL_AVAILABLE;
    struct timeval this_time, last_time;
    double elapsed_time = 0;
    const double dt = 1.0 / SCAN_RATE * 1000; // convert into miliseconds

    // add the file pointer
    FILE *log_file_ptr = NULL;

    if (!mcc118_init_print(num_channels, SCAN_RATE, &address,
                           OPTS_CONTINUOUS, channel_mask, channel_string))
    {
        return -1;
    }
    // Configure and start the scan.
    // Since the continuous option is being used, the samples_per_channel
    // parameter is ignored if the value is less than the default internal
    // buffer size (10000 * num_channels in this case). If a larger internal
    // buffer size is desired, set the value of this parameter accordingly.
    result = mcc118_a_in_scan_start(address, channel_mask, samples_per_channel,
                                    SCAN_RATE, OPTS_CONTINUOUS);
    STOP_ON_ERROR(result, address);

    // time stamp when the scan starts
    gettimeofday(&last_time, NULL);

    print_buffer_info(address);

    #ifdef PRINT_DATA_IN_TERMINAL
    print_header_info(num_channels, channel_string, channel_array);
    #endif
    // create a logging file
    if (!create_mcc118_log_file(&log_file_ptr, channel_mask, channel_name, num_channels))
    {
        stop_and_cleanup(address);
        return -1;
    }

    // Continuously update the display value until enter key is pressed
    do
    {

        usleep(200*1000);
        // Since the read_request_size is set to -1 (READ_ALL_AVAILABLE), this
        // function returns immediately with whatever samples are available (up
        // to user_buffer_size) and the timeout parameter is ignored.
        result = mcc118_a_in_scan_read(address, &read_status, read_request_size,
                                       TIME_OUT, read_buf, user_buffer_size, &samples_read_per_channel);
        STOP_ON_ERROR(result, address);

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

            if (!create_mcc118_log_file(&log_file_ptr, channel_mask, channel_name, num_channels))
            {

                stop_and_cleanup(address);
                return -1;
            }


            printf("\n-----------------------------"
                   "\nMaximal logging time reached.\nOld files were closed.\n");
            // close the files and creat a new file and add the header to the files

            printf("New files created.\n-----------------------------\n\n");

            // update last_time
            last_time = this_time;
        }

#ifdef PRINT_DATA_IN_TERMINAL
        print_data(samples_read_per_channel, num_channels, read_buf);
#endif
        // order a file

    } while ((result == RESULT_SUCCESS) &&
             ((read_status & STATUS_RUNNING) == STATUS_RUNNING) &&
             !enter_press());

    printf("\n\r");

    return 0;
}