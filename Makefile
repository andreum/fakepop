CFLAGS=

all: fakepop

fakepop: fakepop.c
	gcc fakepop.c -o fakepop $(INCLUDE) $(CFLAGS)


