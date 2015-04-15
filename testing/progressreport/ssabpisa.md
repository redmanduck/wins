## Week 12
### April 3rd, 2015

This week I focused on
 
- Keypad
- WiFi caching problem
- Data Collection software to test IMU readings and WiFi 
- 3D printing the package. 

> As a team, we also ran in critical problem with the power supply unit: Raspberry Pi shutdowns abruptly with one the USB wifi dongle plugged in, it draws ~200 mA of current in addition to the ~200mA the Raspberry Pi's single core ARMv6 processor draws. Moreover, the battery datasheet says that the default discharge rate is 0.2C up to 2C. We are not sure what this meant, does it discharge at 0.2C unless you tell it to discharge at 2C via some input resistance value? Or does the C rate depends on something else. This is an open issue we are trying to figure out.  

**The keypad** that we use is a standard 4x4 8-pins keypad, which takes 4 lines input, 4 lines output (Parallel interface). I hooked this up to 8 GPIO pins and wrote a small program to handle keypad input. Siddarth is working on integrating the keypad class to the main program - where it gets assigned its own thread. Next big goal would be to demo the ability to press key on keypad and display it on our Canvas simulator mentioned in the previous progress report.

The main loop looks for keypad event by multiplexing between rows and columns:


	void KeypadHandler::MainLoop() {
  		signal(SIGABRT, &sighandler);
  		signal(SIGTERM, &sighandler);
  		signal(SIGINT, &sighandler);
  		buffer_ = "";
  		int state = 0, old_state = -1, new_state = 0;
  		if(!init_gpio()) {
    		cout << "GPIO Failed to Initialize!";
			while(not terminate_);
  		} else {
			while(not terminate_){
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

`GetEvent()` function used above is simply checking for any of the row input to be on `HIGH`. It returns the row number (1..4) according to which row is pressed. 

Statement ` Wn = (i == 1 ? W1 : (i == 2? W2:  (i == 3? W3: W4)));` above chooses the column that we are pulling `HIGH`. Only one column is pulled high at a time. By intersecting `Wn` and `Rn` we can know exactly which button on the 4x4 matrix is pressed.

To debounce the button, we must sample the button press twice, 1ms apart. If the press changes we assume that it is unstable and discard the event. Otherwise it is valid).

With the row and column number known, we can get the character pressed by accessing `map[r][c]`. `map` shown below:

    char map[4][4]  = {{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};


<figure>
![Caption.](http://ecx.images-amazon.com/images/I/61JPRn6su1L._SL1500_.jpg =320x300)
<figcaption>
	**Figure 1** - 4x4 Universial 16 Key Switch Keypad Keyboard  
</figcaption>
</figure>

As for **WiFi caching problem**, we found that certain tools such as `iw` gives information about `Last Beacon`. When the device scans for access point it either sends out probe requests and wait for probe responses on certain channels (Active Scanning) or it just listen on certain channels and wait for beacons that APs periodically broadcast to be detected (Passive Scanning). In the case of 5 GHz band, it is heavily regulated and in many regions. Devices are not allowed to initiate any radiation on this band [1].

Sid and I spent a significant amount of time on Thursday night from 8 PM to 6 AM (Friday) reverse engineering `iwlib`. We spent the first 5 hours trying to make the device scans more often only to find that it is controlled solely by the driver. So we have two options:

- Modify the driver, make it scan more often.
- Filter scan result by thresholding beacon lifetime

We went with the latter. Firstly, I introduced a new data structure that is a modification of the original `wifi_scan` struct.

    typedef struct duck_scan
    {
      /* Linked list */
      struct duck_scan *	next;

      /* Cell identifiaction */
      int		has_ap_addr;
      sockaddr	ap_addr;		/* Access point address */
    
      /* Other information */
      struct wireless_config	b;	/* Basic information */
      iwstats	stats;			/* Signal strength */
      int		has_stats;
      iwparam	maxbitrate;		/* Max bit rate in bps */
      int		has_maxbitrate;
      
	  char extra[IW_CUSTOM_MAX+1];
    } duck_scan; 

Note the `char extra[IW_CUSTOM_MAX+1];` field. This field holds all the extra information provided by the driver. The `Last beacon` data falls into this category. 

The following addition to the parsing code allows each `wscan` structure (representing one visible AP) to contain this `Last beacon` information: 

    case IWEVCUSTOM:
		if((event->u.data.pointer) && (event->u.data.length))
			memcpy(wscan->extra, event->u.data.pointer, event->u.data.length);
			wscan->extra[event->u.data.length] = '\0';

The following piece of code skip the process of `duck_scan` (wscan) object when its too old. 

	if(ttl > BEACON_TTL){
			//kill it
			std::cout << "Killing result !" << "\n";
			continue;
	}

One of the biggest challenge we had is that the code is poorly documented, commented and variable names are hardly meaningful. 


Next, **Data Collection Software**. I worked on a small Android app that collects acceleration data and gyroscope data. We have collected samples of data but haven't done any processing on it yet. This will be the reference readings that we will be using the calibrate our IMU. Similar thing applies to WiFi data. I built a small button and LED on to the Raspberry Pi, when you press the button it starts scanning for APs and an LED lit up indicating that it is in periodic scanning mode. Hit the button one more time to go into stop mode. When the periodic scanning mode is re-entered the software inserts a delimiter to indicate point of interest. 

We will doing a lot of data processing next week to optimize accuracy of the localization algorithm. 
 
<figure>
![Caption.](light.png =320x300)
<figcaption>
	**Figure 2** - SCAN switch for testing Wifi
</figcaption>
</figure>

<figure>
![Caption.](appuri.png =220x370)
<figcaption>
	**Figure 3** - Gyroscope data collection app
</figcaption>
</figure>


Finally, this week I submitted the 3D stl model to IEEE office to be printed on their makerbot. Our model is slightly larger than the maximum dimension supported on the Formlabs printer we have in EE063. After we receive the printed model from IEEE I will be trying to optimize the model even more. Hopefully at some point we could print it on the Formlabs printer.

[1] Random post on the internet