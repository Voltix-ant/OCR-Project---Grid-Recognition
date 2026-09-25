CC = gcc
CFLAGS = -Wall -Wextra -Werror -fsanitize=address -g
LIBFLAGS = $(shell sdl2-config --cflags --libs) -lSDL2_image
SRC = main.o img_loading.o
EXEC_NAME = app

all: $(SRC)
	$(CC) $^ -o $(EXEC_NAME) $(CFLAGS) $(LIBFLAGS)

%.o: %.c
	$(CC) $< -c -o $@ $(CFLAGS) $(LIBFLAGS)

clean:
	$(RM) *.o $(EXEC_NAME)
