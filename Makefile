CFLAGS+=-Wall -Wno-unused-function -g -DDEBUG
LDFLAGS+=-I. -I../include
export CC CFLAGS LDFLAGS

INSTALL_DIR=install -d -m0755
INSTALL_BIN=install -m0755
INSTALL_DATA=install -m0644
INSTALL_CONF=install -m0644

server:
	$(MAKE) -C server

cli:
	$(MAKE) -C cli

clean:
	$(MAKE) -C cli clean
	$(MAKE) -C server clean
	@-rm -rf cscope* tags

install:
	$(INSTALL_DIR) /etc/clipos
	$(INSTALL_BIN) server/serprobe /usr/bin/
	$(INSTALL_CONF) config/*.conf /etc/clipos/

uninstall:
	@rm -rf /usr/bin/serprobe

.PHONY: cli server install
