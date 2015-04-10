CC=g++
GCC=gcc
winsd.o: winsd.c
	$(GCC) winsd.c -o winsd -lbcm2835
tools:
	$(GCC) monitor.c -o monitor -lbcm2835

clean:
	rm *.o
	rm winsd
