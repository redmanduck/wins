/*
 *  WINS Daemon
 *
 *  Enables getting at SPI, UART and I2C messages.
 *  
 *  Author: ssabpisa
 *
 */

#define WINSD_VER "winsd_1.0"

#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

enum opcode {
    IMU = 'M',
    KEYPAD = 'K',
    HALT = 'H',
    ULTRASONIC='U'
};

int main(){
  //initialize as daemon
  pid_t process_id = 0;
  pid_t sid = 0;
  process_id = fork();
  if(process_id <0){
    printf("Unable to fork\n");
    exit(1);
  }
  if(process_id > 0 ){
    exit(0);
  }
  
  if (!bcm2835_init()) {
    printf("Unable to init bcm2835\n");
    exit(1);
  }

  umask(0); //unmask file mode

  sid = setsid();
  if(sid < 0){
    exit(1);
  }

  chdir("/");

  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
 
  while(1){
    sleep(1);
    printf( "Sending ...\n");
    char mosi[10] = { 0x11, 0x11, 0x11, 0x11, 0x0 , 0x0, 0x0, 0x0, 0x00 };
    char miso[10] = { 0 };

    bcm2835_spi_transfernb(mosi, miso, 4);

    printf("RX: ");
 
    int i = 0;
    for(i = 0; i < 10; i++){
     printf("%x " ,miso[i]);
    }
  
  }
  
 
  printf("\n");

  bcm2835_spi_end();
  bcm2835_close();

}
