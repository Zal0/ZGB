ifeq ($(OS),Windows_NT)
	BUILD_OS := Windows_NT
else
	BUILD_OS := $(shell uname -s)
endif

# Target older macOS version than whatever build OS is for better compatibility
all:
ifeq ($(BUILD_OS),Darwin)
	cp dist/mac/uge2source ./uge2source
	chmod a+x ./uge2source
else ifeq ($(BUILD_OS),Linux)
	cp dist/linux/uge2source ./uge2source
	chmod a+x ./uge2source
else ifeq ($(BUILD_OS),Windows_NT)
	cp dist/windows/uge2source.exe ./uge2source.exe
endif

clean:

