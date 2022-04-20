CC=gcc
CXX=g++
DOXYGEN=doxygen
RELEASE=OFF
XFLAGS=-Wall -Werror -pedantic -I. -D_TIME_BITS=64 -D_FILE_OFFSET_BITS=64
CARGO=cargo
LINK=STATIC
CARGO_FLAGS=
BIN_EXT=
SO_PREFIX=lib
SO_EXT=.so

# example targets:
# x86_64-unknown-linux-gnu
# x86_64-pc-windows-gnu
# i686-pc-windows-gnu
# x86_64-apple-darwin

ifeq ($(TARGET),)
    TARGET_DIR   = target
else
    CARGO_FLAGS += --target $(TARGET)
    TARGET_DIR   = target/$(TARGET)

ifeq ($(patsubst i686-%,i686,$(TARGET)),i686)
    XFLAGS += -m32 -march=i686
else
ifeq ($(patsubst x86_64-%,x86_64,$(TARGET)),x86_64)
    XFLAGS += -m64 -march=x86-64
else
ifeq ($(patsubst armv7-%,armv7,$(TARGET)),armv7)
    XFLAGS += -march=armv7
else
ifeq ($(patsubst armv7a-%,armv7a,$(TARGET)),armv7a)
    XFLAGS += -march=armv7-a
else
ifeq ($(patsubst aarch64-%,aarch64,$(TARGET)),aarch64)
    XFLAGS += -m64 -march=armv8-a
endif
endif
endif
endif
endif

ifeq ($(TARGET),i686-pc-windows-gnu)
    CC  = i686-w64-mingw32-gcc
    CXX = i686-w64-mingw32-g++
else
ifeq ($(TARGET),x86_64-pc-windows-gnu)
    CC  = x86_64-w64-mingw32-gcc
    CXX = x86_64-w64-mingw32-g++
endif
endif

ifeq ($(patsubst %-windows-gnu,windows-gnu,$(TARGET)),windows-gnu)
    XFLAGS   += -Wno-pedantic-ms-format -static-libgcc
    BIN_EXT   = .exe
    SO_PREFIX =
    SO_EXT    = .dll
else
ifeq ($(patsubst %-apple-darwin,apple-darwin,$(TARGET)),apple-darwin)
    CC        = clang
    CXX       = clang++
    XFLAGS   += -Qunused-arguments
    SO_EXT    = .dylib
endif
endif
endif

ifeq ($(RELEASE),ON)
    BUILD_DIR    = $(TARGET_DIR)/release
    XFLAGS      += -O2 -DNDEBUG
    CARGO_FLAGS += --release
else
ifeq ($(RELEASE),OFF)
    BUILD_DIR = $(TARGET_DIR)/debug
    XFLAGS   += -g
else
    $(error illegal value for RELEASE=$(RELEASE))
endif
endif

LIB=$(BUILD_DIR)/libimsz.a
SO =$(BUILD_DIR)/$(SO_PREFIX)imsz$(SO_EXT)
OBJ=$(BUILD_DIR)/examples/imsz.o
BIN=$(BUILD_DIR)/examples/imsz$(BIN_EXT)
OBJXX=$(BUILD_DIR)/examples/imszxx.o
BINXX=$(BUILD_DIR)/examples/imszxx$(BIN_EXT)

ifeq ($(LINK),STATIC)
    # It's fake static linking by direktly linking libimsz.a,
    # i.e. the binary is not actually statically linked, since
    # certain functions in glibc (getpwuid_r and getaddrinfo)
    # require at runtime the shared libraries from the glibc
    # version used for static linking.
ifeq ($(patsubst %-windows-gnu,windows-gnu,$(TARGET)),windows-gnu)
    LIBS    = -lkernel32 -lws2_32 -lbcrypt -ladvapi32 -luserenv
else
    LIBS    = -lgcc_s -lutil -lrt -lpthread -lm -ldl -lc
endif
    DEPS    = $(LIB)
    OBJEX   = $(LIB)
    XFLAGS += -DIMSZ_STATIC
else
ifeq ($(LINK),DYNAMIC)
    LIBS  = -L$(BUILD_DIR) -limsz
    DEPS  = $(SO)
    OBJEX =
else
    $(error illegal value for LINK=$(LINK))
endif
endif

CFLAGS   = $(XFLAGS) -std=c11
CXXFLAGS = $(XFLAGS) -std=c++2a

.PHONY: all clean doc

all: $(BIN) $(BINXX)

doc: imsz.h Doxyfile
	$(DOXYGEN)

$(BIN): $(OBJ) $(DEPS)
	@mkdir -p $(BUILD_DIR)/examples
	$(CC) $(CFLAGS) $< $(OBJEX) $(LIBS) -o $@

$(BINXX): $(OBJXX) $(DEPS)
	@mkdir -p $(BUILD_DIR)/examples
	$(CXX) $(CXXFLAGS) $< $(OBJEX) $(LIBS) -o $@

$(BUILD_DIR)/examples/%xx.o: examples/%.cpp imsz.h
	@mkdir -p $(BUILD_DIR)/examples
	$(CXX) $(CXXFLAGS) $< -o $@ -c

$(BUILD_DIR)/examples/%.o: examples/%.c imsz.h
	@mkdir -p $(BUILD_DIR)/examples
	$(CC) $(CFLAGS) $< -o $@ -c

$(SO): $(LIB)

$(LIB): src/lib.rs Cargo.toml
	$(CARGO) build $(CARGO_FLAGS)

clean:
	rm $(OBJ) $(BIN) $(OBJXX) $(BINXX) || true
	$(CARGO) clean
