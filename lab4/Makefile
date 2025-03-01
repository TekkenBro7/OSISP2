CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = lab4

all: $(TARGET)

$(TARGET): main.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) counter.txt

.PHONY: all clean
