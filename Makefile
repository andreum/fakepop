INCLUDE=-I/opt/local/include/glib-2.0
CFLAGS=

fakepop: fakepop.c
	gcc fakepop.c -o fakepop $(INCLUDE) $(CFLAGS)

