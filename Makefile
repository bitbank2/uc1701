CFLAGS=-c -Wall -O2

all: libuc1701.a

libuc1701.a: uc1701.o fonts.o
	ar -rc libuc1701.a uc1701.o fonts.o ;\
	sudo cp libuc1701.a /usr/local/lib ;\
	sudo cp uc1701.h /usr/local/include

fonts.o: fonts.c
	$(CC) $(CFLAGS) fonts.c

uc1701.o: uc1701.c
	$(CC) $(CFLAGS) uc1701.c

clean:
	rm *.o libuc1701.a

