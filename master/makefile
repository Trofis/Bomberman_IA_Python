SOURCES= $(wildcard *.c)
HEADERS= $(wildcard *.h)
HEADERS2= $(wildcard ../*.h)
OBJ= $(SOURCES:.c=.o)
CC= gcc
CFLAGS= -O -Wall 

master: $(OBJ) $(HEADERS) $(HEADERS2)
	$(CC) $(CFLAGS) -o master $(OBJ)  -lm -lpthread


%.o: %.c $(HEADERS) $(HEADERS2)
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm -f *.o *~
