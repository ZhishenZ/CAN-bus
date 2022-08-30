PROG = main#continuous_scan
MCC_118_LIB = mcc118_continuous_scan
# DAQ_LIB = daqhat_utils
CC = gcc
FLAGS = -lpthread -ldaqhats
CFLAGS = -Wall -I/usr/local/include -g

$(PROG): $(PROG).c $(MCC_118_LIB).c
		$(CC) $(MCC_118_LIB).c $(PROG).c $(FLAGS) $(CFLAGS) -o $(PROG)

.PHONY: clean

clean:
		rm $(PROG)