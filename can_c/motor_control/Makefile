PROG = motor_control
LIB = can_lib
CC = gcc

$(PROG): $(PROG).c
		$(CC) $(LIB).h $(LIB).c $(PROG).c -lpthread -o $(PROG)

clean:
		rm $(PROG)