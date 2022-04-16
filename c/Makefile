CC=gcc
RELEASE=OFF
CFLAGS=-Wall -std=c11 -I.
CARGO=cargo
LINK=STATIC

ifeq ($(RELEASE),ON)
    BUILDDIR=target/release
    CFLAGS += -O2
    CARGO_FLAGS=--release
else
ifeq ($(RELEASE),OFF)
    BUILDDIR=target/debug
    CFLAGS += -g
    CARGO_FLAGS=
else
    $(error illegal value for RELEASE=$(RELEASE))
endif
endif

LIB=$(BUILDDIR)/libimsz.a
SO =$(BUILDDIR)/libimsz.so
OBJ=$(BUILDDIR)/examples/imsz.o
BIN=$(BUILDDIR)/examples/imsz

ifeq ($(LINK),STATIC)
    # It's fake static linking by direktly linking libimsz.a,
    # i.e. the binary is not actually statically linked, since
    # certain functions in glibc (getpwuid_r and getaddrinfo)
    # require at runtime the shared libraries from the glibc
    # version used for static linking.
    LIBS=-lgcc_s -lutil -lrt -lpthread -lm -ldl -lc
    DEPS=$(LIB)
    OBJEX=$(LIB)
    CFLAGS+=-DIMSZ_STATIC
else
ifeq ($(LINK),DYNAMIC)
    LIBS=-L$(BUILDDIR) -limsz
    DEPS=$(SO)
    OBJEX=
else
    $(error illegal value for LINK=$(LINK))
endif
endif

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ) $(DEPS)
	@mkdir -p $(BUILDDIR)/examples
	$(CC) $(CFLAGS) $< $(OBJEX) $(LIBS) -o $@

$(BUILDDIR)/examples/%.o: examples/%.c imsz.h
	@mkdir -p $(BUILDDIR)/examples
	$(CC) $(CFLAGS) $< -o $@ -c

$(SO): $(LIB)

$(LIB): src/lib.rs Cargo.toml
	$(CARGO) build $(CARGO_FLAGS)

clean:
	rm $(OBJ) $(BIN) || true
	$(CARGO) clean
