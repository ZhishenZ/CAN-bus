PROG = continuous_scan
CAN_BUS_LIB = mcc118_continuous_scan
# DAQ_LIB = daqhat_utils
CC = gcc
FLAGS = -lpthread -ldaqhats
CFLAGS = -Wall -I/usr/local/include -g

$(PROG): $(PROG).c
		$(CC) $(CAN_BUS_LIB).c $(PROG).c $(FLAGS) $(CFLAGS) -o $(PROG)

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
