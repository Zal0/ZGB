OBJDIR = ../Release
BINDIR = ../bin

LCC = c:/gbdk/bin/lcc.exe
BGB = c:/gbdk/bin/bgb/bgb.exe

CFLAGS = -DUSE_SFR_FOR_REG -I..\include
#-Wa-l -Wl-m -Wl-j 
LFLAGS = -DUSE_SFR_FOR_REG
CC = $(LCC)

current_dir = $(shell pwd)
CLASSES := $(foreach dir,.,$(notdir $(wildcard $(dir)/*.c)))
OBJS = $(CLASSES:%.c=$(OBJDIR)/%.o)

all: $(BINDIR)/$(PROJECT_NAME).gb

$(BINDIR):
	mkdir $(BINDIR)
	
$(OBJDIR):
	mkdir $(OBJDIR)

%.s: %.ms
	maccer -o $@ $<

$(OBJDIR)/%.o: %.c $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<
	
$(BINDIR)/$(PROJECT_NAME).gb: $(OBJS) $(BINDIR)
	$(CC) $(LFLAGS) -o $(BINDIR)/$(PROJECT_NAME).gb $(OBJS)

clean:
	rm -rf $(BINDIR)/*.gb
	rm -rf $(OBJDIR)/*.o
	rm -rf .map
	rm -rf .lst
	
run: $(BINDIR)/$(PROJECT_NAME).gb
	$(BGB) $(BINDIR)/$(PROJECT_NAME).gb
	