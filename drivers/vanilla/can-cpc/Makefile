CFLAGS += -I../can -I../cpc-usb -fPIC
LDFLAGS += -L../can -L../cpc-usb

PREFIX = /usr/local

libcan-cpc.so: can_cpc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o libcan-cpc.so \
	can_cpc.c -lcpc
	ln -sf $(PWD)/libcan-cpc.so ../can/libcan.so

install: libcan-cpc.so
	install -c -m 755 libcan-cpc.so $(PREFIX)/lib/libcan.so

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
