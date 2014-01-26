# Simple makefile to build inih as a static library using g++

SRC = ini.c deps.c main.c
OBJ = $(SRC:.c=.o)
OUT = cdep
INCLUDES = 
CCFLAGS = -g -O2 -Wall -Werror
CC = gcc

default: $(OUT)

.c.o:
	$(CC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT)
