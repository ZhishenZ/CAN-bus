PROG = main#continuous_scan
CAN_BUS_LIB = can_lib
MCC_118_LIB = mcc118_continuous_scan
# DAQ_LIB = daqhat_utils
CC = gcc
FLAGS = -lpthread -ldaqhats -lm `pkg-config --cflags --libs gtk+-3.0` -export-dynamic
CFLAGS = -Wall -Wno-deprecated-declarations -Wno-format-security -I/usr/local/include -g

$(PROG): $(PROG).c
		$(CC) -Wno-format $(CAN_BUS_LIB).c $(MCC_118_LIB).c $(PROG).c $(FLAGS) $(CFLAGS) -o $(PROG)


.PHONY: clean

clean:
		rm $(PROG)


# NAME = continuous_scan
# OBJ = $(NAME).o
# LIBS = -ldaqhats
# CFLAGS = -Wall -I/usr/local/include -g
# CC = gcc
# EXTENSION = .c

# all: $(NAME)

# %.o: %$(EXTENSION) $(DEPS)
# 	$(CC) -c -o $@ $< $(CFLAGS)

# $(NAME): $(OBJ)
# 	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# .PHONY: clean

# clean:
# 	@rm -f *.o *~ core $(NAME)
