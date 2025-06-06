# TODO: add exact compiler name here
CC = gcc 
# TODO: optimize flags, add support for libpynq-extended
CFLAGS = -Wall -O2 -I./lib
LDFLAGS = -L./lib -lpynq -static

# Find all .c files in the root except for the entrypoint
LIB_HEADERS := $(wildcard *.h)
LIB_OBJS := $(LIB_HEADERS:.h=.o)

all: rover

rover: initialize.o $(LIB_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o rover

.PHONY: all clean
