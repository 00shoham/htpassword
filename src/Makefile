BIN=\
  config-test\
  htpasswd-test\
  policy-test\
  htpassword\

all: $(BIN)

UTILDIR=../../idanlib/src
UTILSLIB=$(UTILDIR)/utils.a

LUADIR := $(shell $(UTILDIR)/lua-dir.sh)
LUALIB := $(shell $(UTILDIR)/lua-ver.sh)

INCLUDES=$(LUADIR) -I$(UTILDIR)
LIBS=$(UTILSLIB) -lm -lpthread -luuid $(LUALIB) -lcurl

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
