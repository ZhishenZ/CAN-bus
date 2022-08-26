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
extern struct can_frame frame;
// extern clock_t clk_t;

int can_send_init( struct ifreq ifr, struct sockaddr_can addr); 

void Can_Sdo_Write_NULL(uint16_t can_id);

void close_can();

void Can_Sdo_Write( uint16_t can_id, uint16_t addr, uint8_t sub_addr, uint32_t data);

void Can_Sdo_Read( uint16_t can_id, uint16_t addr, uint8_t sub_addr);

void Can_Pdo_Write(uint16_t can_id, uint8_t* data_array, uint8_t array_len);

void *time_watcher(void *args);

/**
 * @brief read the response from motor and check is there is an error
 *        time out error when the motor does not response
 * 
 */
void Can_Sdo_read_and_check(uint16_t can_id, uint16_t addr, uint8_t sub_addr);

void *Pdo_logging_thread(void *args);

void start_Pdo_logging();

void stop_Pdo_logging();

void Can_Sdo_write_while_Pdo_logging(uint16_t can_id, uint16_t addr, uint8_t sub_addr, uint32_t data);

void Can_Sdo_read_and_check_while_Pdo_logging(uint16_t can_id, uint16_t addr, uint8_t sub_addr);

int create_log_file();

void *pdo_time_watcher(void *args);

void get_Pdo_response(uint32_t *motor_current,
                      uint32_t *motor_position,
                      uint32_t *load_cell_voltage);

#endif // CAN_SEND_H