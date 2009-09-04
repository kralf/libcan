CFLAGS += -I../can -I../cpc-usb -I../../../posix/config -fPIC
LDFLAGS += -L../can -L../../../posix/config -lconfig -L../cpc-usb -lcpc

PREFIX = /usr/local

libcan-cpc.so: ../can/can.h ../can/can.c can_cpc.h can_cpc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o libcan-cpc.so \
	../can/can.c can_cpc.c
	ln -sf $(PWD)/libcan-cpc.so ../can/libcan.so

install: libcan-cpc.so
	install -c -m 755 libcan-cpc.so $(PREFIX)/lib/libcan.so

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
