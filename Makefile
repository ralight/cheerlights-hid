CC=gcc
INSTALL=install
prefix=/usr/local
CFLAGS=-Wall -ggdb -O2 -I.
LDADD=-lmosquitto
LDFLAGS=

.PHONY : all test clean install

all : cheerlights-hid

cheerlights-hid.o : cheerlights-hid.c
	$(CC) -c $(CFLAGS) -fPIC -o $@ $<

dbus.o: dbus.c
	$(CC) -c $(CFLAGS) `pkg-config gio-2.0 --cflags` -o $@ $<

cheerlights-hid : cheerlights-hid.o dbus.o
	$(CC) -o $@ $^ ${LDFLAGS} ${LDADD} `pkg-config gio-2.0 --libs`

clean :
	-rm -f *.o cheerlights-hid

install : cheerlights-hid
	$(INSTALL) -d ${DESTDIR}${prefix}/bin/
	$(INSTALL) cheerlights-hid ${DESTDIR}${prefix}/bin/cheerlights-hid
	$(INSTALL) -d ${DESTDIR}/etc/systemctl/system
	$(INSTALL) cheerlights-hid.service ${DESTDIR}/etc/systemctl/system/cheerlights-hid.service

