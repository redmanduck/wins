/*
 *  WINS Daemon
 *
 *  Enables getting at SPI, UART and I2C messages.
 *  
 *  Author: ssabpisa
 *
 */

#define WINSD_VER "winsd_1.0"
#define NBYTES 6
#define NSECONDS 3

#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

enum opcode {
    IMU = 'M',
    KEYPAD = 'K',
    HALT = 'H',
    ULTRASONIC='U',
    POSITION='P'
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
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2048);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

  FILE *f = fopen("/home/pi/wins/raspi/daemon/dump.txt", "w");
  if (f==NULL){
     printf("Errror opening");
     exit(1);
  }
  
  int var = 0;
  char mosi[NBYTES] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  char miso[NBYTES] = { 0 };
  int j = 0;
  for(j=0; j < NSECONDS; j++){
  sleep(1);
  printf("%d: going on...\n", j);
  var = 0;

  while(var<1000){
    usleep(10000);

    int i = 0;
    for(i = 0; i<2; i++){
      bcm2835_spi_transfer(IMU);
    }
    for(i = 0; i<NBYTES; i++){
      miso[i]=bcm2835_spi_transfer(mosi[i]);
    }
    //bcm2835_spi_transfernb(mosi, miso, NBYTES);

 
    fprintf(f,"var = %u, RX: ", var);
    printf("var = %u, RX: ", var);
    for(i = 0; i < NBYTES; i++){
     printf("%x " ,miso[i]);
     fprintf(f,"%x " ,miso[i]);
    }
    fprintf(f, " TX: ");
    printf(" TX: ");
    for(i = 0; i < NBYTES; i++){
     fprintf(f,"%x " ,mosi[i]);
     printf("%x " ,mosi[i]);
     mosi[i]+=6;
    }
     fprintf(f, "\n", mosi[0]);
     printf("\n", mosi[0]);
    var += 1;
  
  }
  }
  printf("done\n");
  fprintf(f,"\n");
  fclose(f);
  bcm2835_spi_end();
  bcm2835_close();

}
