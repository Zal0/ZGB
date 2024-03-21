
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


SRCDIR = src
OBJDIR = obj
BINDIR = bin
MKDIRS = $(OBJDIR) $(BINDIR) $(PACKDIR)
CFILES = $(wildcard $(SRCDIR)/*.c)
COBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CFILES))
BIN = $(BINDIR)/vgm2data

$(BIN): $(COBJ)
	$(CC) $(LFLAGS) -o $(BIN) $(COBJ)
	strip $@$(EXT)


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)*.o
	rm -f $(BINDIR)*.exe
	rm -f $(BIN)

test:
	./vgm2data.py res/sound_effect1.vgm -o ref_sfx1.c -v > ref_log1.txt
	./vgm2data.py res/sound_effect2.vgm -o ref_sfx2.c -v > ref_log2.txt
	./vgm2data.py res/sound_effect3.vgm -o ref_sfx3.c -v > ref_log3.txt
	bin/vgm2data res/sound_effect1.vgm -o test_sfx1.c -v > test_log1.txt
	bin/vgm2data res/sound_effect2.vgm -o test_sfx2.c -v > test_log2.txt
	bin/vgm2data res/sound_effect3.vgm -o test_sfx3.c -v > test_log3.txt


# create necessary directories after Makefile is parsed but before build
# info prevents the command from being pasted into the makefile
$(info $(shell mkdir -p $(MKDIRS)))

