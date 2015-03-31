#include <bcm2835.h>
#include <iostream>
#include <signal.h>
#include <string>
#include <mutex>

#define W1 RPI_BPLUS_GPIO_J8_11
#define W2 RPI_BPLUS_GPIO_J8_36
#define W3 RPI_BPLUS_GPIO_J8_10
#define W4 RPI_BPLUS_GPIO_J8_08

#define R1 RPI_BPLUS_GPIO_J8_40
#define R2 RPI_BPLUS_GPIO_J8_38
#define R3 RPI_BPLUS_GPIO_J8_35
#define R4 RPI_BPLUS_GPIO_J8_12
using namespace std;
class KeypadHandler{
	public:
			KeypadHandler();
			mutex buffer_mutex;
};

string buffer;
void processButton(int r,int c);
void sighandler(int sig);
int get_event();	
