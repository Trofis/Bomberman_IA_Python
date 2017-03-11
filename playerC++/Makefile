CC=g++
CFLAGS=-W -Wall -ansi -pedantic
LDFLAGS=
EXEC=player
SRC= $(wildcard *.cpp)
OBJ= $(SRC:.cpp=.o)

all: $(EXEC)

%.o: %.cpp
	$(CC) -o $@ -c $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $(EXEC) $(OBJ) $(LDFLAGS)


.PHONY: clean mrproper

clean:
	rm -rf *.o *~

mrproper: clean
	rm -rf $(EXEC)
