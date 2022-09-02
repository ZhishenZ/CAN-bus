#ifndef CAN_SEND_H
#define CAN_SEND_H

/*Global Variables*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <time.h>
#include <pthread.h>

extern int s;
extern struct can_frame frame; /* struct to store the CAN information */

int can_send_init( struct ifreq ifr, struct sockaddr_can addr); 

void Can_Sdo_Write_NULL(uint16_t can_id);

void close_can(FILE* file_ptr);

void Can_Sdo_Write( uint16_t can_id, uint16_t addr, uint8_t sub_addr, uint32_t data);

void Can_Sdo_Read( uint16_t can_id, uint16_t addr, uint8_t sub_addr);

void Can_Pdo_Write(uint16_t can_id, uint8_t* data_array, uint8_t array_len);

/**
 * @brief A thread as time watcher, if there is a time out issue, it will stop the program 
 */
void *time_watcher(void *args);

/**
 * @brief read the response from motor and check is there is an error
 *        time out error when the motor does not response
 */
void Can_Sdo_read_and_check(uint16_t can_id, uint16_t addr, uint8_t sub_addr);

FILE* create_log_file(FILE *log_file_p);

void *pdo_time_watcher(void *args);

void get_Pdo_response(uint32_t *motor_current,
                      uint32_t *motor_position,
                      uint32_t *load_cell_voltage);


void get_and_record_Pdo_response(uint32_t *motor_current,
                                 uint32_t *motor_position,
                                 uint32_t *load_cell_voltage,
                                 int data_recording_active,
                                 FILE *log_file_p);

#endif // CAN_SEND_H