CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c99 -D_DEFAULT_SOURCE -Iinclude -g
TARGET = my_ls

OBJ = src/main.o src/options.o src/entry.o src/list.o \
      src/operands.o src/sort.o src/print.o src/util.o

HEADERS = include/options.h include/entry.h include/list.h \
          include/sort.h include/print.h include/util.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

$(OBJ): $(HEADERS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
