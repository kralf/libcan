CFLAGS += -I../can -I../epos -fPIC
LDFLAGS += -L../can -L../epos

PREFIX = /usr/local

libcan-serial.so: can_serial.c
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -shared -o libcan-serial.so \
	can_serial.c
	ln -sf $(PWD)/libcan-serial.so ../can/libcan.so

install: libcan-serial.so
	install -c -m 644 libcan-serial.so $(PREFIX)/lib/libcan.so

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
