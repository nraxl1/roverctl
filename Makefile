# TODO: Add specific compiler name later
CC = gcc
# TODO: Change compiler flags for performance or compilation speed later
CFLAGS = -Wall -O2 -I./lib
LDFLAGS = -L./lib -lpynq -static

# initialize.c is the hardcoded entrypoint for the application
SRC := $(filter-out initialize.c, $(wildcard *.c))
OBJ = $(SRC:.c=.o)
OBJ_TEST = $(SRC:.c=.test.o)

roverctl: $(OBJ)
    $(CC) -o $@ $^ $(LDFLAGS)

roverctl-test: CFLAGS += -DTEST_BUILD
roverctl-test: $(OBJ_TEST)
    $(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

%.test.o: %.c
    $(CC) $(CFLAGS) -DTEST_BUILD -c $< -o $@

clean:
    rm -f *.o *.test.o roverctl roverctl-test

.PHONY: all clean