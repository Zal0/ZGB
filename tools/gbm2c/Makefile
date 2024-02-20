NAME	:= gbm2c

SOURCES	:= gbm2c.cpp ../gbr2c/gbrParser.cpp

OBJS	:= $(SOURCES:%.cpp=%.o)

CC	:= g++
CFLAGS	:= -static -I../gbr2c

all:	$(NAME)

$(NAME):
	$(CC) $(CFLAGS) -o $@ $(SOURCES)

clean:
	rm -f $(OBJS) $(NAME)
