CFLAGS += -I../can -I../epos -I../../../posix/serial -fPIC
LDFLAGS += -L../can -L../epos -L../../../posix/serial -lserial

PREFIX = /usr/local

libcan-serial.so: can_serial.h can_serial.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o libcan-serial.so \
	can_serial.c
	ln -sf $(PWD)/libcan-serial.so ../can/libcan.so

install: libcan-serial.so
	install -c -m 755 libcan-serial.so $(PREFIX)/lib/libcan.so

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
