OPT = -I/usr/local/include -L/usr/local/lib -lwiringPi
all: lcdtest lcdclock

lcdtest: clean
	g++ $(OPT) -g -o lcdtest lcdtest.cpp

lcdclock: clean
	g++ $(OPT) -g -o lcdclock lcdclock.cpp

clean:
	rm -rf ./lcdtest
	rm -rf ./lcdclock
