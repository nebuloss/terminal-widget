EXEC=main
CFLAGS=-lm
SRC=$(wildcard *.c)

all:COMPILE RUN
COMPILE: $(SRC)
	gcc $^ -o $(EXEC) $(CFLAGS)
RUN:COMPILE
	./$(EXEC)
