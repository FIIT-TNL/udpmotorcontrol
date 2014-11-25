CFLAGS=-W -Wall

all: motorcontrol

clean:
	rm -f motorcontrol

motorcontrol: motorcontrol.c
	$(CC) $(CFLAGS) -o $@ $^

.PHONY: all clean
