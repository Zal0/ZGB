# NAME	:= gbr2c

# SOURCES	:= gbr2c.cpp gbrParser.cpp

# OBJS	:= $(SOURCES:%.cpp=%.o)

# CC	:= g++
# CFLAGS	:= -static

# all:	$(NAME)

# $(NAME):
# 	$(CC) $(CFLAGS) -o $@ $(SOURCES)

# clean:
# 	rm -f $(OBJS) $(NAME)


# *****************************************************
# Variables to control Makefile operation

# Might want when doing linux -> win cross build
# LFLAGS = -s -static

# Prefix to add to the standard tools.  Usefull for a standard gcc
# cross-compile.
TOOLSPREFIX =
ifeq ($(OS_TARGET),Win_x32)
	TOOLSPREFIX = i686-w64-mingw32-
endif
ifeq ($(OS_TARGET),Win_x64)
	TOOLSPREFIX = x86_64-w64-mingw32-
endif


CXX = $(TOOLSPREFIX)g++
CXXFLAGS = -Os -g -I../gbr2c -Wno-unused-result # -Wall  -Wextra -pedantic
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

TARGET = gbr2c


SRCS := gbr2c.cpp gbrParser.cpp
OBJS := $(SRCS:%.cpp=%.o)

$(TARGET): $(OBJS)
	$(CXX) $(LFLAGS) -o $(TARGET) $(OBJS)
	strip $@$(EXT)

# The main.o target can be written more simply

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f *.exe
	rm -f $(TARGET)
