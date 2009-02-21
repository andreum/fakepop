INCLUDE=-I/opt/local/include/glib-2.0
CFLAGS=-g

all: fakepop fakepop2

fakepop: fakepop.c
	gcc fakepop.c -o fakepop $(INCLUDE) $(CFLAGS)

fakepop2: fakepop2.c
	gcc fakepop2.c -o fakepop2 $(INCLUDE) $(CFLAGS)

