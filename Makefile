SHARDCLIENT_VERSION = 0.0.1
LUA_VERSION = 5.1

## Linux/BSD
#PREFIX ?=          /usr/local
#LDFLAGS +=         -shared

## OSX
PREFIX ?=          /usr/local
LDFLAGS +=         -bundle -undefined dynamic_lookup

LUA_INCLUDE_DIR ?= $(PREFIX)/include
LUA_LIB_DIR ?=     $(PREFIX)/lib/lua/$(LUA_VERSION)

CFLAGS ?=          -std=c99 -g -O3 -Wall -pedantic
override CFLAGS += -fpic -I$(LUA_INCLUDE_DIR) -DVERSION=\"$(SHARDCLIENT_VERSION)\"

CC ?= gcc
INSTALL ?= install

.PHONY: all clean install

all: clientsharding.so

clientsharding.so: rbtree.o murmurhash.o clientsharding.o
		$(CC) $(LDFLAGS) -o $@ $^
install:
	 $(INSTALL) -d $(DESTDIR)/$(LUA_LIB_DIR)
	 $(INSTALL) clientsharding.so  $(DESTDIR)/$(LUA_LIB_DIR)
clean:
	rm -rf *.o *.so


