CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -O2
TARGET  = float_point_add
SRC     = float_point_add.c
OBJ     = $(SRC:.c=.o)

.PHONY: all clean run
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
