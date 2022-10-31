BIN=\
  config-test\
  htpasswd-test\
  policy-test\
  htpassword\

all: $(BIN)

LUADIR := $(shell /usr/bin/find /usr/include -type d -name 'lua*')
LUAPARTS := $(subst /, ,$(LUADIR))
LUAVER := $(lastword $(LUAPARTS))

UTILDIR=../../idanlib/src
UTILSLIB=$(UTILDIR)/utils.a
INCLUDES=-I$(LUADIR) -I$(UTILDIR)

CC=gcc
CFLAGS=-Wall -O2 -g $(INCLUDES)
LIBS=-lm -lpthread -luuid -lcurl
SHAREDOBJ=\
  hibp.o\
  config.o\
  api.o\
  password-history.o\
  password-file.o\
  htpasswd-wrapper.o\
  password-policy.o

SHAREDLIB=\
  $(UTILDIR)/utils.a

%.o: %.c $(wildcard *.h)
	$(CC) $(CFLAGS) -c $<

$(BIN): % : %.o $(SHAREDOBJ)
	$(CC) $(CFLAGS) -o $@ $< $(SHAREDOBJ) $(SHAREDLIB) $(LIBS)

install: $(BIN)
	chmod 755 install.sh
	./install.sh

clean: NULL
	rm -f $(SHAREDOBJ) $(BIN)
	rm -f *.o

NULL:
