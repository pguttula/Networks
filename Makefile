CC = gcc
CFLAGS = -g -w -Wall -Wno-return-type -Wno-implicit-function-declaration -Wno-implicit-int -Wno-incompatible-library-redeclaration -Wno-unused-variable

all: gbn arq sr

gbn:
	$(CC) $(CFLAGS) prog2_gbn.c -o gbn.out
arq:
	$(CC) $(CFLAGS) prog2_arq.c -o arq.out

sr:
	$(CC) $(CFLAGS) prog2_sr.c -o sr.out

clean:
	-rm -f *.o *.out
