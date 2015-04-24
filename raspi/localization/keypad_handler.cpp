#include <bcm2835.h>
#include <iostream>
#include <signal.h>

#include "common_utils.h"
#include "global.h"
#include "keypad_handler.h"
#include "log.h"

namespace wins {

#define W1 RPI_BPLUS_GPIO_J8_37
#define W2 RPI_BPLUS_GPIO_J8_35
#define W3 RPI_BPLUS_GPIO_J8_33
#define W4 RPI_BPLUS_GPIO_J8_31

#define R1 RPI_BPLUS_GPIO_J8_40
#define R2 RPI_BPLUS_GPIO_J8_38
#define R3 RPI_BPLUS_GPIO_J8_36
#define R4 RPI_BPLUS_GPIO_J8_32

thread KeypadHandler::keypad_thread_;
atomic_bool terminate_(false);

void sighandler(int sig)
{
    FILE_LOG(logINFO) << "Signal " << sig << ": Keypad Gracefully closing" << endl;
    terminate_ = true;
		bcm2835_gpio_write(W1, LOW);
		bcm2835_gpio_write(W2, LOW);
		bcm2835_gpio_write(W3, LOW);
		bcm2835_gpio_write(W4, LOW);
}

bool init_gpio(){
   if (!bcm2835_init()) return false;

	  bcm2835_gpio_fsel(W1, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_fsel(W2, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_fsel(W3, BCM2835_GPIO_FSEL_OUTP);
		bcm2835_gpio_fsel(W4, BCM2835_GPIO_FSEL_OUTP);

	  bcm2835_gpio_fsel(R1, BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_fsel(R2, BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_fsel(R3, BCM2835_GPIO_FSEL_INPT);
		bcm2835_gpio_fsel(R4, BCM2835_GPIO_FSEL_INPT);

		bcm2835_gpio_set_pud(R1, BCM2835_GPIO_PUD_DOWN);
		bcm2835_gpio_set_pud(R2, BCM2835_GPIO_PUD_DOWN);
		bcm2835_gpio_set_pud(R3, BCM2835_GPIO_PUD_DOWN);
		bcm2835_gpio_set_pud(R4, BCM2835_GPIO_PUD_DOWN);
		//detect edge
    bcm2835_gpio_hen(R1);
    bcm2835_gpio_hen(R2);
    bcm2835_gpio_hen(R3);
    bcm2835_gpio_hen(R4);

    return true;
}

/*
char map[4][4]  = {
	{'A','B','C','D'},
	{'3','6','9','#'},
	{'2','5','8','0'},
	{'1','4','7','*'}
};*/
//char map[4][4]  = {
//  {'D','A','B','C'},
//  {'#','3','6','9'},
//  {'0','2','5','8'},
//  {'*','1','4','7'}
//};
char map[4][4]  = {
  {'1','-','7','4'},
  {'2','0','8','5'},
  {'3','#','9','6'},
  {'A',',','C','B'}
};


void KeypadHandler::ProcessButton(int row, int col) {
	if(row == -1 || col == -1) return;

	FILE_LOG(logKEYPAD) << "Row Pressed " << row << " Col " << col << "\n";
  FILE_LOG(logKEYPAD) << "Key value:" << map[row-1][col] << "\n";

	//get lock
	lock_guard<mutex> lock(buffer_mutex_);
	buffer_ += map[row-1][col];
  Global::SetEventFlag(WINS_EVENT_KEYPRESS);
}

void KeypadHandler::MainLoop() {
  signal(SIGABRT, &sighandler);
  signal(SIGTERM, &sighandler);
  signal(SIGINT, &sighandler);
  buffer_ = "";
  int state = 0, old_state = -1, new_state = 0;
  if(!init_gpio()) {
    FILE_LOG(logERROR) << "GPIO Failed to Initialize!\n";
		while(not terminate_.load());
  } else {
		while(not terminate_.load()){
      //FILE_LOG(logKEYPAD) << "=====LOOPING\n";
			for(int i = 0; i < 4; i++){
				int Wn = (i == 1 ? W1 : (i == 2? W2:  (i == 3? W3: W4)));
				bcm2835_gpio_write(Wn, HIGH);
				new_state = GetEvent();
				if(old_state != new_state){
					//this press is new, we will consider it
					delay(1);
					//gather
					state = 0;
					state = GetEvent();	//compare it to the press 1ms later

					//if it changes, we discard it
					if(state == new_state){
						ProcessButton(state, i);
					} else {
            //FILE_LOG(logKEYPAD) << "DISCARDED!!! - " << new_state << "\n";
          }

					old_state = state;

			  }
				//clear all GPIO states
				bcm2835_gpio_clr(W1);
				bcm2835_gpio_clr(W2);
				bcm2835_gpio_clr(W3);
				bcm2835_gpio_clr(W4);

				delay(4); //delay for clearing GPIO

			}
			delay(300);
		}
    bcm2835_close();
  }
}

KeypadHandler::KeypadHandler() {
}

int KeypadHandler::GetEvent(){
  int state = -1;
  if (bcm2835_gpio_eds(R1)) {
    state = 1;
    bcm2835_gpio_set_eds(R1);
  } else if(bcm2835_gpio_eds(R2)){
    state = 2;
    bcm2835_gpio_set_eds(R2);
  } else if(bcm2835_gpio_eds(R3)){
    state = 3;
    bcm2835_gpio_set_eds(R3);
  } else if(bcm2835_gpio_eds(R4)){
    state = 4;
    bcm2835_gpio_set_eds(R4);
  }
  //FILE_LOG(logKEYPAD) << "-------------- GPIO READ\n" << state;
  return state;
}

KeypadHandler& KeypadHandler::GetInstance() {
  static KeypadHandler keypad_handler;
  return keypad_handler;
}

void KeypadHandler::StartThread() {
  if (not keypad_thread_.joinable()) {
    keypad_thread_ = thread(&KeypadHandler::MainLoop,
        &KeypadHandler::GetInstance());
  }
}
void KeypadHandler::TerminateThread() {
  sighandler(SIGTERM);
  keypad_thread_.join();
}

char KeypadHandler::GetChar() {
  while (true) {
    {
      lock_guard<mutex> lock(buffer_mutex_);
      if (buffer_.size() > 0)
        break;
    }
    Global::BlockForEvent(WINS_EVENT_KEYPRESS);
  }

  lock_guard<mutex> lock(buffer_mutex_);
  // Read the buffer and mark as read.
  char input_char = buffer_[0];
  buffer_.erase(0, 1);

  return input_char;
}

void KeypadHandler::FakeStringEnter(string s) {
	lock_guard<mutex> lock(buffer_mutex_);
  for (auto c : s) {
    buffer_ += c;
  }
  Global::SetEventFlag(WINS_EVENT_KEYPRESS);
}

}
