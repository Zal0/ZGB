NAME	:= gbr2c

SOURCES	:= gbr2c.cpp gbrParser.cpp

OBJS	:= $(SOURCES:%.cpp=%.o)

CC	:= g++
CFLAGS	:= -static

all:	$(NAME)

$(NAME):
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	rm -f $(OBJS) $(NAME)
