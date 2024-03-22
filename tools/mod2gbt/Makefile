
# Prefix to add to the standard tools.  Usefull for a standard gcc
# cross-compile.
TOOLSPREFIX =
ifeq ($(OS_TARGET),Win_x32)
	TOOLSPREFIX = i686-w64-mingw32-
endif
ifeq ($(OS_TARGET),Win_x64)
	TOOLSPREFIX = x86_64-w64-mingw32-
endif


CC = $(TOOLSPREFIX)gcc
CCFLAGS = -Os -g  # -Wall -Wextra -pedantic
LFLAGS = -g

ifeq ($(OS),Windows_NT)
	BUILD_OS := Windows_NT
else
	BUILD_OS := $(shell uname -s)
endif

# Target older macOS version than whatever build OS is for better compatibility
ifeq ($(BUILD_OS),Darwin)
	export MACOSX_DEPLOYMENT_TARGET=10.10
endif


# Static flag for windows cross and native builds
ifeq ($(TOOLSPREFIX),i686-w64-mingw32-)
#   prefix will automatically be .exe for cross build
	EXT = .exe
	LFLAGS += -static 
endif

ifeq ($(OS),Windows_NT)
	EXT = .exe
	LFLAGS += -static 
endif


# ****************************************************
# Targets needed to bring the executable up to date

TARGET = mod2gbt


SRCS := mod2gbt.c
OBJS := $(SRCS:%.c=%.o)

$(TARGET): $(OBJS)
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJS)
	strip $@$(EXT)

# The main.o target can be written more simply

%.c.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f *.exe
	rm -f $(TARGET)
