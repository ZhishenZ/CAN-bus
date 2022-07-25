/**
 * @file two_boards.c
 * @author Zhishen ZHANG (zhishen.zhang@knorr-bremse.com)
 * @brief This program collects the data required by two mcc118s synchronously.
 *        The collected data is stored in the csv file.
 *        The bottom board sends the CLK signal to the other mcc 118 boards,
 *        which collect sensor data at the frequency of the master board.
 *
 *        Before running this program, read the EEPROMs with the MCC DAQ HAT Manager
 *        and connect the CLK terminals on the MCC 118 boards together.
 * @version 0.1
 * @date 2022-05-04
 * @copyright Copyright (c) 2022
 */
//#include "../../daqhats_utils.h" 
#include "daqhats_utils.h"
#include <time.h>
#include <errno.h>
#include <sys/time.h>

#define DEVICE_COUNT 2            // here we set two devices.
#define MASTER 0                  // set the master's address to be 0, which is the board at the bottom layer with out jumper.
#define SAMPLES_PER_CHANNEL 200 // the number of samples per channel
#define SAMPLES_RATE 1000         // the sample rate
#define NUM_CHANNELS 8            // the number of channels of a board //things to change
#define LOGGING_TIME 3600         // time to record the data in seconds 

//selected channel of device/board 0
#define SEL_CHAN_DEV_0  CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5 | CHAN6 | CHAN7
//selected channel of device/board 1
#define SEL_CHAN_DEV_1  CHAN0 | CHAN1 | CHAN2 | CHAN3 | CHAN4 | CHAN5 | CHAN6 | CHAN7



//#define CURSOR_SAVE "\x1b[s"
// #define CURSOR_RESTORE "\x1b[u"
// #define RESULT_SUCCESS 0

int get_hat_addresses(uint8_t address[]);

int scan_init(uint8_t address[], int chan_count[],uint32_t options[],
             uint8_t chan_mask[],int chans[][NUM_CHANNELS], char options_str[][256],
             double sample_rate, double actual_sample_rate,  char chan_display[][32]);


int creat_log_files(FILE** log_file_ptr_0, FILE** log_file_ptr_1);


int add_log_files_header(FILE** log_file_ptr_0, FILE** log_file_ptr_1,
                        char chan_name [2][8][32], uint8_t chan_mask[DEVICE_COUNT]);


void initial_print(double sample_rate, double actual_sample_rate, uint8_t address[],
                   char chan_display[][32], char options_str[][256] );


int write_log_file(FILE* log_file_ptr_0, FILE* log_file_ptr_1,
                   double read_buf[][SAMPLES_PER_CHANNEL*NUM_CHANNELS*5],
                   int numberOfDevices, uint32_t* samples_read,int numberOfChannels,
                   double* elapsed_time_dev0, double* elapsed_time_dev1, const double dt);


void display_data_in_terminal(uint32_t samples_read[], int chan_count[], 
                              int total_samples_read[], int chans[][NUM_CHANNELS],
                              int sample_idx, double data[][sample_idx]);


int check_overrun(uint16_t scan_status_all);


void clean_up_and_stop(uint8_t address[]);
// FILE* open_log_file (char* path, FILE* log_file_ptr);


int main(void)
{
    // HAT device addresses - determined at runtime
    uint8_t address[DEVICE_COUNT];

    // mcc118_a_in_scan_start() variables
    // all channels open
    uint8_t chan_mask[DEVICE_COUNT] = { SEL_CHAN_DEV_0, SEL_CHAN_DEV_1 };

    char chan_name [2][8][32] = 
    {{"Chan0_Dev0","Chan1_Dev0","Chan2_Dev0","Chan3_Dev0","Chan4_Dev0","Chan5_Dev0","Chan6_Dev0","Chan7_Dev0"},
    {"Chan0_Dev1","Chan1_Dev1","Chan2_Dev1","Chan3_Dev1","Chan4_Dev1","Chan5_Dev1","Chan6_Dev1","Chan7_Dev1"}};



    // struct timeval tv_pre, tv_cur;
    // double scanning_period = 0;// in miliseconds
    // double elapsed_time = 0;// in milisecond

    double elapsed_time_dev0 = 0;     // for device 0 in miliseconds
    double elapsed_time_dev1 = 0;     // for device 1 in miliseconds
    double dt = 1.0/SAMPLES_RATE*1000;  // convert into miliseconds

    struct timeval this_time, last_time;

    uint32_t options[DEVICE_COUNT] = {
        OPTS_CONTINUOUS, /*OPTS_EXTTRIGGER,*/
        OPTS_CONTINUOUS | OPTS_EXTCLOCK};

    double sample_rate = SAMPLES_RATE; // Samples per second
    // mcc118_a_in_scan_status() and mcc118_a_in_scan_read() variables
    uint16_t scan_status[DEVICE_COUNT] = {0};

    int buffer_size = SAMPLES_PER_CHANNEL * NUM_CHANNELS * 5;// make sure that there is enough space for the buffer. 
    // double data[DEVICE_COUNT][buffer_size];

    uint32_t samples_read[DEVICE_COUNT] = {0};
    // int32_t samples_to_read = 500;
    // double timeout = 5; // seconds
    // uint32_t samples_available = 0;

    int result = 0;
    int result1 = 0;
    int chan_count[DEVICE_COUNT];
    int chans[DEVICE_COUNT][NUM_CHANNELS];
    int device = 0;
    int sample_idx = 0;
    // int i = 0;
    int total_samples_read[DEVICE_COUNT] = {0};
    const int data_display_line_count = DEVICE_COUNT * 4;
    uint16_t is_running = 0;
    // uint16_t is_triggered = 0;
    uint16_t scan_status_all = 0;

    FILE* log_file_ptr_0 = NULL;
    FILE* log_file_ptr_1 = NULL;
    
    char chan_display[DEVICE_COUNT][32];
    char options_str[DEVICE_COUNT][256];

    double actual_sample_rate = 0.0;

    // Determine the addresses of the devices to be used
    if (get_hat_addresses(address) != 0)
    {
        return -1;
    }

    // STOP_ON_ERROR(scan_init(address, chan_count, options, chan_mask, chans, options_str,
    // sample_rate, actual_sample_rate, chan_display));

    result = scan_init(address, chan_count, options, chan_mask, chans, options_str,
                            sample_rate, actual_sample_rate, chan_display);
    STOP_ON_ERROR(result);

    initial_print(sample_rate, actual_sample_rate, address,
                  chan_display,  options_str);

    // Start the scan
    for (device = 0; device < DEVICE_COUNT; device++)
    {
        result = mcc118_a_in_scan_start(address[device], chan_mask[device],
                                        0, sample_rate, options[device]); 
        STOP_ON_ERROR(result);
    }

    // time stamp of the starting time 
    gettimeofday(&last_time,NULL);


    result = mcc118_a_in_scan_status(address[MASTER], &scan_status[MASTER],
                                     NULL);
    STOP_ON_ERROR(result);
    is_running = (scan_status[MASTER] & STATUS_RUNNING);

    if (is_running)
    {
        printf("Acquiring data ... Press 'Enter' to abort\n\n");
        // Create blank lines where the data will be displayed.
        for (int i = 0; i <= data_display_line_count; i++)
        {
            printf("\n");
        }
        // Move the cursor up to the start of the data display
        printf("\x1b[%dA", data_display_line_count + 1);
        // Save the cursor position
        // printf(CURSOR_SAVE);
    }else{
        printf("Aborted\n\n");
    }


    if(!creat_log_files(&log_file_ptr_0, &log_file_ptr_1))
        goto stop;

    //add headers
    if(!add_log_files_header(&log_file_ptr_0, &log_file_ptr_1,chan_name,chan_mask))
        goto stop;

    // gettimeofday(&tv_pre, NULL); comment this out for a more precise time step

    //-------------------------------------starts the loop ------------------------------

    while (is_running)
    {
        usleep(200000);//sleep for 200000 microseconds/200 ms /0.2 s


        

        double data[DEVICE_COUNT][buffer_size];

        // for (device = 0; device < DEVICE_COUNT; device++)
        // {
        //     // Read data
        //     result = mcc118_a_in_scan_read(address[device],&scan_status[device], READ_ALL_AVAILABLE, 
        //                                    0, data[device],buffer_size, &samples_read[device]);

        //     STOP_ON_ERROR(result);
        //     // Check for overrun on any one device
        //     scan_status_all |= scan_status[device];
        //     // Verify the status of all devices is running
        //     is_running &= (scan_status[device] & STATUS_RUNNING);
        // }



        result = mcc118_a_in_scan_read(address[0],&scan_status[0], READ_ALL_AVAILABLE, 
                                           0, data[0],buffer_size, &samples_read[0]);

        // add a time stamp
        result1 = mcc118_a_in_scan_read(address[1],&scan_status[1], READ_ALL_AVAILABLE, 
                                           0, data[1],buffer_size, &samples_read[1]);


        //get the time step 
        // gettimeofday(&tv_cur, NULL); comment this out for a more precise time step
        // scanning_period = (tv_cur.tv_sec-tv_pre.tv_sec)*1000 + (tv_cur.tv_usec-tv_pre.tv_usec)*0.001;

        // tv_pre = tv_cur;

        // add another time stamp
        STOP_ON_ERROR(result);
        STOP_ON_ERROR(result1);
        // Check for overrun on any one device
        scan_status_all |= scan_status[0];
        scan_status_all |= scan_status[1];
        // Verify the status of all devices is running
        is_running &= (scan_status[0] & STATUS_RUNNING);
        is_running &= (scan_status[1] & STATUS_RUNNING);

        // printf("is_running: %d\n",is_running);

        if(!check_overrun(scan_status_all)) break;
        /* Store the data*/

        if(!write_log_file(log_file_ptr_0,log_file_ptr_1, data, 
                           DEVICE_COUNT,&samples_read[0], NUM_CHANNELS,
                           &elapsed_time_dev0,&elapsed_time_dev1,dt)){

              fprintf(stderr, "\nWrite log file failed.\n");
              break;
        };

        
        //if the logging time is reached,
            //close the previous file and open a new file
        gettimeofday(&this_time,NULL);

        if(this_time.tv_sec-last_time.tv_sec>=LOGGING_TIME){
            
            fclose(log_file_ptr_0);
            fclose(log_file_ptr_1);
            printf("\n-----------------------------"
                   "\nMaximal logging time reached.\nOld files were closed.\n");
            //close the files and creat a new file and add the header to the files
            if(!creat_log_files(&log_file_ptr_0, &log_file_ptr_1))
                goto stop;

            printf("New files created.\n-----------------------------\n\n");

            if(!add_log_files_header(&log_file_ptr_0, &log_file_ptr_1,chan_name,chan_mask))
                goto stop;
            //update last_time
            last_time = this_time;
            total_samples_read[0] = 0;
            total_samples_read[1] = 0;

        }


        // elapsed_time+=scanning_period;
        display_data_in_terminal(samples_read, chan_count, total_samples_read, chans, sample_idx, data);

        if (enter_press())
        {
            printf("Aborted\n\n");
            break;
        }
    }

    //-------------------------------------ends the loop ------------------------------


stop:
    // Stop and cleanup
    clean_up_and_stop(address);
    printf("All devices are stopped.\n");

    //CLose the log files!!!
    fclose(log_file_ptr_0);
    fclose(log_file_ptr_1);
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
int write_log_file(FILE* log_file_ptr_0, FILE* log_file_ptr_1,
                   double read_buf[DEVICE_COUNT][SAMPLES_PER_CHANNEL * NUM_CHANNELS * 5],
                   int numberOfDevices, uint32_t* samples_read/*get from scan function*/,int numberOfChannels,
                   double* elapsed_time_dev0, double* elapsed_time_dev1, const double dt)
{
    int write_status = 0;
    char str[1000];
    char buf[32];
    char daytime[50];

    
    struct timeval tv;
    
    gettimeofday(&tv, NULL);
    int micro = tv.tv_usec;

    strftime(buf,32,"%Y-%m-%d %H:%M:%S",localtime(&tv.tv_sec));

    sprintf(daytime, "%s.%06d,",buf,micro);

    // double dt_dev0 = scanning_period / *samples_read; comment this out for a more precise time step
    // double dt_dev1 = scanning_period / *(samples_read+1); comment this out for a more precise time step
    strcpy(str, "");

    // Device 0
    for (int i = 0; i < *samples_read; i++){
        strcpy(str, "");

        // sprintf(buf,"%2.6f,", (dt_dev0*i + elapsed_time));
        // strcat(str, buf);
        // last time step of the block
        strcat(str,daytime);

        sprintf(buf,"%2.6f,", *elapsed_time_dev0);
        strcat(str, buf);
        *elapsed_time_dev0 += dt;
        // Write a sample for each channel in the scan.
        for ( int j = 0; j < numberOfChannels; j++)
        {
            // Convert the data sample to ASCII
            sprintf(buf,"%2.6f,", read_buf[0][numberOfChannels*i+j]);
            // Add the data sample to the string to be written.
            strcat(str, buf);
        }

        str[strlen(str)-1] = '\0';

        write_status = fprintf(log_file_ptr_0, "%s\n", str);
        if (write_status <= 0)
        {
            fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
            break;
        }

    }

    // Flush the file to ensure all data is written.
    fflush(log_file_ptr_0);
    
    // Device 1
    for (int i = 0; i < *(samples_read+1); i++){
        strcpy(str, "");

        strcat(str,daytime);

        // sprintf(buf,"%2.6f,", (dt_dev1*i + elapsed_time));
        // strcat(str, buf); 

        sprintf(buf,"%2.6f,", *elapsed_time_dev1);
        strcat(str, buf);
        *elapsed_time_dev1 += dt;
        // Write a sample for each channel in the scan.
        for ( int j = 0; j < numberOfChannels; j++)
        {
            // Convert the data sample to ASCII
            sprintf(buf,"%2.6f,", read_buf[1][numberOfChannels*i+j]);
            // Add the data sample to the string to be written.
            strcat(str, buf);
        }  

        str[strlen(str)-1] = '\0';    
        
        write_status = fprintf(log_file_ptr_1, "%s\n", str);
        if (write_status <= 0)
        {
            fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
            break;
        }

    }

    // Flush the file to ensure all data is written.
    fflush(log_file_ptr_1);

    // Return the error code.
    return write_status;
}



void display_data_in_terminal(uint32_t samples_read[], int chan_count[], 
                              int total_samples_read[], int chans[DEVICE_COUNT][NUM_CHANNELS],
                              int sample_idx,double data[DEVICE_COUNT][sample_idx]){

    char display_string[256] = "";
    char data_display_output[1024] = "";

    for (int device = 0; device < DEVICE_COUNT; device++)
        {
            strcpy(data_display_output, "");

            sprintf(display_string, "HAT %d:    samples_read:%d\n", device, samples_read[device]);
            strcat(data_display_output, display_string);

            // Display the header row for the data table
            strcat(data_display_output, "  Samples Read    Scan Count");
            for (int i = 0; i < chan_count[device]; i++)
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
                for (int i = 0; i < chan_count[device]; i++)
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


void clean_up_and_stop(uint8_t address[]){

    int result;

    // Stop and cleanup
    for ( int device = 0; device < DEVICE_COUNT; device++)
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
                   char chan_display[DEVICE_COUNT][32], char options_str[DEVICE_COUNT][256] ){
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




int check_overrun(uint16_t scan_status_all){

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



int scan_init(uint8_t address[], int chan_count[DEVICE_COUNT],uint32_t options[DEVICE_COUNT],
             uint8_t chan_mask[DEVICE_COUNT],int chans[DEVICE_COUNT][NUM_CHANNELS], char options_str[DEVICE_COUNT][256],
             double sample_rate, double actual_sample_rate,  char chan_display[DEVICE_COUNT][32]){

    for (int device = 0; device < DEVICE_COUNT; device++)
    {
        // Open a connection to each device
        int result = mcc118_open(address[device]);
        //STOP_ON_ERROR(result);
        if(result != 0) return result;
        // channel_mask is used by the library function mcc118_a_in_scan_start.
        // The functions below parse the mask for display purposes.
        chan_count[device] = convert_chan_mask_to_array(chan_mask[device],
                                                        chans[device]);
        convert_chan_mask_to_string(chan_mask[device], chan_display[device]);
        convert_options_to_string(options[device], options_str[device]);
    }

    int result = mcc118_a_in_scan_actual_rate(chan_count[MASTER], sample_rate,
                                          &actual_sample_rate);
    //STOP_ON_ERROR(result);
    if(result != 0) 
        return result;

    return 0;
}


int creat_log_files(FILE** log_file_ptr_0, FILE** log_file_ptr_1){

    // set the pathLogFiles
    char path_LogFiles[512];
    struct tm* current_tm;
    time_t this_time_t;

    char time_string[32];
    char log_filename_device_0[512] = "";
    char log_filename_device_1[512] = "";


    getcwd(path_LogFiles, sizeof(path_LogFiles));
    strcat(path_LogFiles, "/LogFiles/");

    // Initialize the output file
    this_time_t = time(NULL);
    current_tm = localtime(&this_time_t);
    //csv file name
    sprintf(time_string, "%02d_%02d_%02d_%02d_%02d_%02d_%s",current_tm->tm_year +1900, 
    (current_tm->tm_mon+1), current_tm->tm_mday, current_tm->tm_hour, 
    current_tm->tm_min,current_tm->tm_sec, current_tm->tm_zone);
    
    strcat(log_filename_device_0,path_LogFiles);
    strcat(log_filename_device_0,time_string);
    strcat(log_filename_device_0,"_DevAddr0.csv");

    strcat(log_filename_device_1,path_LogFiles);
    strcat(log_filename_device_1,time_string);
    strcat(log_filename_device_1,"_DevAddr1.csv");

    // Open the log file
    *log_file_ptr_0 = fopen(log_filename_device_0, "w");
    *log_file_ptr_1 = fopen(log_filename_device_1, "w");


    if (*log_file_ptr_0 == NULL || *log_file_ptr_1 == NULL ){
        printf("Open file errno = %d reason %s \n",errno,strerror(errno));
        printf("Please create a directory called \"LogFiles\" "
               "in the directory \"/three_boards.\" \n\n");

        return 0;
    }

    return 1;
}



int add_log_files_header(FILE** log_file_ptr_0, FILE** log_file_ptr_1,
                        char chan_name [2][8][32], uint8_t chan_mask[DEVICE_COUNT]){

    // First device
    char str_0[256] = "Day Time,Time(ms),";
    for (int i = 0; i<NUM_CHANNELS; i++){
        if(chan_mask[0] & (0x01<<i)){
            strcat(str_0 ,chan_name[0][i]);
            strcat(str_0 , ",");
            //printf("chane_name: %s\n",chan_name[0][i]);
        }
    }
    str_0[strlen(str_0)-1]='\0';
    int write_status = fprintf(*log_file_ptr_0, "%s\n", str_0);
    if (write_status <= 0)
    {
        fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
        //break;
    }
    // Flush the file to ensure all data is written.
    fflush(*log_file_ptr_0);


    //second device
    strcpy(str_0 , "");
    strcat(str_0 , "Day Time,Time(ms),");

    for (int i = 0; i<NUM_CHANNELS; i++){
        if(chan_mask[1] & (0x01<<i)){
            strcat(str_0 ,chan_name[1][i]);
            strcat(str_0 , ",");
            //printf("chan_name: %s\n",chan_name[0][i]);
        }
    }
    str_0[strlen(str_0)-1]='\0';
    write_status = fprintf(*log_file_ptr_1, "%s\n", str_0);
    if (write_status <= 0)
    {
        fprintf(stderr, "\nError: Write the ASCII scan data to the file.\n");
        return 0;
    }

    // Flush the file to ensure all data is written.
    fflush(*log_file_ptr_1);
    return 1;

};