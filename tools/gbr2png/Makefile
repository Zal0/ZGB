NAME	:= gbr2png

SOURCES	:= gbr2png.cpp ./lodepng/lodepng.cpp ../gbr2c/gbrParser.cpp

OBJS	:= $(SOURCES:%.cpp=%.o)

CC	:= g++
CFLAGS	:= -static -I../gbr2c -I./lodepng

all:	$(NAME)

$(NAME):
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	rm -f $(OBJS) $(NAME)
