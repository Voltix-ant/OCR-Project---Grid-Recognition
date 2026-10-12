CC = gcc
CFLAGS = -Wall -Wextra -fsanitize=address -g
LIBFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_image -lm
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
EXEC_NAME = app

all: $(OBJ)
	$(CC) $^ -o $(EXEC_NAME) $(CFLAGS) $(LIBFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LIBFLAGS)

clean:
	$(RM) $(OBJ) $(EXEC_NAME)
