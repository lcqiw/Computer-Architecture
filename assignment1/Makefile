CC     = gcc
CFLAGS = -std=c11 -Wall -Werror -pedantic -g

TARGET = float_bits

all: $(TARGET)

$(TARGET): float_bits.o
	$(CC) $(CFLAGS) -o $@ $^

float_bits.o: float_bits.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o
