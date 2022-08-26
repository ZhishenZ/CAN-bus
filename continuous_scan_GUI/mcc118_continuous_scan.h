#ifndef MCC188_CONTINUOUS_SCAN
#define MCC188_CONTINUOUS_SCAN

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <daqhats/daqhats.h>
#include "congif.h"
#include <time.h>
#include <errno.h>
#include <sys/time.h>

// Channel definitions
#define CHAN0 0x01 << 0
#define CHAN1 0x01 << 1
#define CHAN2 0x01 << 2
#define CHAN3 0x01 << 3
#define CHAN4 0x01 << 4
#define CHAN5 0x01 << 5
#define CHAN6 0x01 << 6
#define CHAN7 0x01 << 7
#define MAX_CHAN_ARRAY_LENGTH 32

extern int data_recording_active;

// Timeout definitions
#define WAIT_INDEFINITELY -1
#define RETURN_IMMEDIATELY 0

// Read definitions
#define READ_ALL_AVAILABLE -1

// Macros to handle error checking
#define STOP_ON_ERROR(result, address) \
    {                                  \
        if (result != RESULT_SUCCESS)  \
        {                              \
            print_error(result);       \
            stop_and_cleanup(address); \
        }                              \
    }

void print_buffer_and_header_info(uint8_t address, uint8_t num_channels,
                                  char channel_string[512], int *channel_array);

void stop_and_cleanup(uint8_t address);

int mcc118_init_print(uint8_t num_channels, double scan_rate, uint8_t *address,
                      uint32_t options, uint8_t channel_mask, char channel_string[512]);

int select_hat_device(uint16_t hat_filter_id, uint8_t *address);

int convert_chan_mask_to_array(uint32_t channel_mask, int chans[]);

void convert_chan_mask_to_string(uint32_t channel_mask, char *chans_str);

void print_error(int result);

/****************************************************************************
 * User input functions
 ****************************************************************************/
void flush_stdin(void);

void convert_options_to_string(uint32_t options, char *options_str);

int write_log_file(FILE *log_file_ptr, uint8_t num_channels, double *read_buf,
                   uint32_t *samples_read, double *elapsed_time, const double dt);

void print_data(uint32_t samples_read_per_channel, uint8_t num_channels, double read_buf[]);

int check_overrun(uint16_t read_status);

int create_mcc118_log_file(FILE **log_file_ptr, uint8_t channel_mask, char channel_name[][32], uint8_t num_channels);

void print_buffer_info(uint8_t address);

void print_header_info(uint8_t num_channels,char channel_string[512], int *channel_array);

int mcc118_continuous_scan(void);


int mcc118_continuous_scan_new(void);

#endif