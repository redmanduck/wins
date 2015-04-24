#include "pad.h"


bool forever = true;

void sighandler(int sig)
{
    cout<< "Signal " << sig << ": Gracefully closing" << endl;
		forever = false;
		bcm2835_gpio_write(W1, LOW);
		bcm2835_gpio_write(W2, LOW);
		bcm2835_gpio_write(W3, LOW);
		bcm2835_gpio_write(W4, LOW);
}

void init_gpio(){
   if (!bcm2835_init()) return;

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
}

int main(void) {
    signal(SIGABRT, &sighandler);
		signal(SIGTERM, &sighandler);
		signal(SIGINT, &sighandler);
		buffer = "";
		int state = 0, old_state = -1, new_state = 0;
    init_gpio();
		while(forever){
			for(int i = 0; i < 4; i++){
				int Wn = (i == 1 ? W1 : (i == 2? W2:  (i == 3? W3: W4)));
				bcm2835_gpio_write(Wn, HIGH);
				new_state = get_event();
				if(old_state != new_state){
					//this press is new, we will consider it
					delay(1);
					//gather
					state = 0;
					state = get_event();	//compare it to the press 1ms later

					//if it changes, we discard it
					if(state == new_state){
						processButton(state, i);
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

char map[4][4]  = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}
};

void processButton(int row, int col){
	if(row == -1 || col == -1) return;

	cout << "Row Pressed " << row << " Col " << col << "\n";
  cout << "Key value:" << map[row-1][col] << "\n";
	//get lock
	//lock_guard<mutex> lock(buffer_mutex);
	//buffer.append(map[row-1][col]);
	buffer += map[row-1][col];
}

int get_event(){
		  int state = -1;
		  if (bcm2835_gpio_eds(R1))
				{
					state = 1;
					bcm2835_gpio_set_eds(R1);
				}else if(bcm2835_gpio_eds(R2)){
					state = 2;

					bcm2835_gpio_set_eds(R2);
				}else if(bcm2835_gpio_eds(R3)){
					state = 3;
					bcm2835_gpio_set_eds(R3);
				}else if(bcm2835_gpio_eds(R4)){
					state = 4;
					bcm2835_gpio_set_eds(R4);
	      }
      cout << "state -> " << state;
			return state;
}
