PROG = motor_control_GUI#continuous_scan
CAN_BUS_LIB = can_lib
MCC_118_LIB = mcc118_continuous_scan
# DAQ_LIB = daqhat_utils
CC = gcc
FLAGS = -lpthread -ldaqhats -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
CFLAGS = -Wall -Wno-deprecated-declarations -Wno-format-security -I/usr/local/include -g

$(PROG): $(CAN_BUS_LIB).c $(MCC_118_LIB).c main.c config.h
		$(CC) -Wno-format $(CAN_BUS_LIB).c $(MCC_118_LIB).c main.c $(FLAGS) $(CFLAGS) -o $(PROG)


.PHONY: clean

clean:
		rm $(PROG)
