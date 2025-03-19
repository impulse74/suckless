CC = gcc
CFLAGS = -Os -Wall
LDFLAGS = -lX11 -lxkbfile
PREFIX = /usr/local
BINDIR = $(PREFIX)/bin

.PHONY: all clean install uninstall

all: statusbar

statusbar: statusbar.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

install: statusbar
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 statusbar $(DESTDIR)$(BINDIR)/statusbar

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/statusbar

clean:
	rm -f statusbar
