
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
BINDIR = .
MKDIRS = $(OBJDIR) $(BINDIR) $(PACKDIR)
CFILES = $(wildcard $(SRCDIR)/*.c)
COBJ = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(CFILES))
BIN = $(BINDIR)/fxhammer2data

$(BIN): $(COBJ)
	$(CC) $(LFLAGS) -o $(BIN) $(COBJ)
	strip $@$(EXT)


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o
	rm -f $(BINDIR)/*.exe
	rm -f $(BIN)

runtest:
# gb
	python3 reference/fxhammer2data.py test/hammered.sav -i myid -d 4 -c -b 4 -o test/ref1_gb.c  > test/ref_log1_gb.txt
	./fxhammer2data                    test/hammered.sav -i myid -d 4 -c -b 4 -o test/test1_gb.c > test/test_log1_gb.txt
	python3 reference/fxhammer2data.py test/hammered.sav -c -p                -o test/ref2_gb.c  > test/ref_log2_gb.txt
	./fxhammer2data                    test/hammered.sav -c -p                -o test/test2_gb.c > test/test_log2_gb.txt
	python3 reference/fxhammer2data.py test/hammered.sav --number=3           -o test/ref3_gb.c  > test/ref_log3_gb.txt
	./fxhammer2data                    test/hammered.sav --number=3           -o test/test3_gb.c > test/test_log3_gb.txt
	diff test/ref1_gb.c test/test1_gb.c
	diff test/ref1_gb.h test/test1_gb.h
	diff test/ref2_gb.c test/test2_gb.c
	diff test/ref2_gb.h test/test2_gb.h
	diff test/ref3_gb.c test/test3_gb.c
	diff test/ref3_gb.h test/test3_gb.h
# psg
	python3 reference/fxhammer2data.py test/hammered.sav --system=psg -i myid -d 4 -c -b 4 -o test/ref1_psg.c  > test/ref_log1_psg.txt
	./fxhammer2data                    test/hammered.sav --system=psg -i myid -d 4 -c -b 4 -o test/test1_psg.c > test/test_log1_psg.txt
	python3 reference/fxhammer2data.py test/hammered.sav --system=psg -c -p                -o test/ref2_psg.c  > test/ref_log2_psg.txt
	./fxhammer2data                    test/hammered.sav --system=psg -c -p                -o test/test2_psg.c > test/test_log2_psg.txt
	python3 reference/fxhammer2data.py test/hammered.sav --system=psg --number=3           -o test/ref3_psg.c  > test/ref_log3_psg.txt
	./fxhammer2data                    test/hammered.sav --system=psg --number=3           -o test/test3_psg.c > test/test_log3_psg.txt
	diff test/ref1_psg.c test/test1_psg.c
	diff test/ref1_psg.h test/test1_psg.h
	diff test/ref2_psg.c test/test2_psg.c
	diff test/ref2_psg.h test/test2_psg.h
	diff test/ref3_psg.c test/test3_psg.c
	diff test/ref3_psg.h test/test3_psg.h


# create necessary directories after Makefile is parsed but before build
# info prevents the command from being pasted into the makefile
$(info $(shell mkdir -p $(MKDIRS)))

