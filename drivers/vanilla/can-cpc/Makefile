CFLAGS += -I../can -I../cpc-usb
LDFLAGS += -L../can -L../cpc-usb

libcan-cpc.so: can_cpc.c
	$(CC) $(CFLAGS) $(LDFLAGS) -Wall -shared -o libcan-cpc.so \
	can_cpc.c -lcpc

install: libcan-cpc.so
	install -c -o root -g root -m 644 libcan-cpc.so /usr/lib

clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
