TARGET = enmon
CC = gcc
CFLAGS = -Wall -pedantic -std=gnu11 -O2
LIBS = -framework IOKit -framework CoreFoundation

.PHONY: all clean

all: $(TARGET)

clean:
	-rm -f $(TARGET) bridge.o ft260.o hid.o main.o sht20.o util.o

bridge.o: bridge.c
	$(CC) $(CFLAGS) -c $< -o $@

ft260.o: ft260.c
	$(CC) $(CFLAGS) -c $< -o $@

hid.o: hid.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

sht20.o: sht20.c
	$(CC) $(CFLAGS) -c $< -o $@

util.o: util.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): bridge.o ft260.o hid.o main.o sht20.o util.o
	$(CC) bridge.o ft260.o hid.o main.o sht20.o util.o $(LIBS) -o $@
