CFLAGS=-g -Iinclude/ -I. -I/usr/include/pixman-1
LDLIBS=-L.

LIB = libdraw.a
BIN = test

SRC_libdraw = $(wildcard libdraw/*.c)
OBJ_libdraw = $(SRC_libdraw:.c=.o)

OBJ_ALL = $(OBJ_libdraw)

all: $(LIB) $(BIN)

$(BIN) : % : %.o

test: LDLIBS += -ludev -linput -ldraw -lpixman-1
test: input.o libdraw.a

libdraw.a : $(OBJ_libdraw)

$(LIB):
	rm -f $@
	ar rc $@ $^
	ranlib $@

clean:
	rm -f *.o $(BIN) $(LIB) $(OBJ_ALL)
