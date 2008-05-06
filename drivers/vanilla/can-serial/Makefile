CFLAGS += -I../can -I../epos
LDFLAGS += -L../can -L../epos

libcan-serial.so: can_serial.c
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -shared -o libcan-serial.so \
	can_serial.c

install: libcan-serial.so
	install -c -o root -g root -m 644 libcan-serial.so /usr/lib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
