CC=gcc
CFLAGS=-I.
LNK=-lbcm2835
spi: spi.o 
	$(CC) -o spi spi.o $(CFLAGS) $(LNK)
clean:
	rm *.o
	rm spi
