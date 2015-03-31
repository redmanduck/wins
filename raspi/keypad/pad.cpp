#include <bcm2835.h>
#include <iostream>
#include <signal.h>

#define W1 RPI_BPLUS_GPIO_J8_11
#define W2 RPI_BPLUS_GPIO_J8_36
#define W3 RPI_BPLUS_GPIO_J8_10
#define W4 RPI_BPLUS_GPIO_J8_08

#define R1 RPI_BPLUS_GPIO_J8_40
#define R2 RPI_BPLUS_GPIO_J8_38
#define R3 RPI_BPLUS_GPIO_J8_35
#define R4 RPI_BPLUS_GPIO_J8_12


using namespace std;
bool forever = true;

void processButton(int jsd);
void sighandler(int sig);
int get_event();

void sighandler(int sig)
{
    cout<< "Signal " << sig << ": Gracefully closing" << endl;
		forever = false;
			
		bcm2835_gpio_write(W1, LOW);
		bcm2835_gpio_write(W2, LOW);
		bcm2835_gpio_write(W3, LOW);
		bcm2835_gpio_write(W4, LOW);

}


int main(void) {
    signal(SIGABRT, &sighandler);
		signal(SIGTERM, &sighandler);
		signal(SIGINT, &sighandler);
    
		int state = 0, old_state = -1, new_state = 0;
    if (!bcm2835_init()) return 1;
	
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
		//setup 
		bcm2835_gpio_write(W1, HIGH);
		bcm2835_gpio_write(W2, HIGH);
		bcm2835_gpio_write(W3, HIGH);
		bcm2835_gpio_write(W4, HIGH);

		//detect edge
    bcm2835_gpio_hen(R1); 
    bcm2835_gpio_hen(R2); 
    bcm2835_gpio_hen(R3);
    bcm2835_gpio_hen(R4); 

		while(forever){
	//		cout << new_state << " " << old_state << " " << state << "\n";
			new_state = get_event();
			
			if(old_state != new_state){
				delay(1);
				//gather
				state = 0;
			  state = get_event();	

				//check stable
				if(state == new_state){	
					processButton(state);
				}

				old_state = state;
	
		  }
			delay(100);
		}

    bcm2835_close();

}

void processButton(int jsd){
			if(jsd != -1){
				cout << "Row Pressed " << jsd << "\n";
				jsd = -1;  //invalidate Just pu(s)he(d)
		  }
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
			return state;
}
