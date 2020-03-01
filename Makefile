TARGET = enmon
CC = gcc
CFLAGS = -Wall -pedantic -std=gnu11 -O2
LIBS = -framework IOKit -framework CoreFoundation

.PHONY: all clean

all: $(TARGET)

clean:
	-rm -f $(TARGET) main.o hid.o

hid.o: hid.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): main.o hid.o
	$(CC) main.o hid.o $(LIBS) -o $@
