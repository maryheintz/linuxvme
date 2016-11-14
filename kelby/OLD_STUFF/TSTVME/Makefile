CC 	= gcc
LIB     = libcctvme.a 
CFLAGS  = -Wall

all: vmetst example5 example4_bis example4

vmetst: vmetst.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIB) 

example5: example5.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIB) 

example4_bis: example4_bis.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIB) 

example4: example4.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIB) 

clean:
	rm -f vmetst example5 example4_bis example4
