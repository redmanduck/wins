#define WINSD_VER "monitor_1.0"
#define NBYTES 6
#define NSECONDS 3
#include <time.h>

#include <bcm2835.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>

enum opcode {
    IMU = 'M',
    ACCEL = 'A',
    GYRO = 'G',
    KEYPAD = 'K',
    HALT = 'H',
    ULTRASONIC='U',
    POSITION='P',
    VALID = 'V',
    ERROR = 'E',
		BAT = 'B',
};

int main(){


  if (!bcm2835_init()) {
		//printf("oops, could not init bcm2835\n");
		return 1;
  }

  //printf("Starting..\n");
  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_4096);   //4096); //2048);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

  int var = 0;
  char mosi[NBYTES] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
  char miso[NBYTES] = { 0 };
  
  char RX = '0';
  char TX = '0';
  int i = 0;
  int ct = 0;
  int err = 0;
  int op_err = 0;
	clock_t total = clock();
	int packets = 0;
	int seconds = 0;
	unsigned char bat1 = 0;
	unsigned char bat2 = 0;
	while(seconds<60){
		packets=0;
		TX = BAT;
    RX = bcm2835_spi_transfer(TX);
    RX = bcm2835_spi_transfer(TX);
    while(RX != BAT){
        op_err++;
				usleep(1000000);
				RX = bcm2835_spi_transfer(TX);
				printf("%x(%c) NEQ\n", RX, RX);
    }
    RX = bcm2835_spi_transfer(TX);
		bat1 = RX;
    RX = bcm2835_spi_transfer(TX);
		bat2 = RX;
  while(packets < 500){
		packets++;
    TX = ACCEL;
		//printf("%d\n", packets);
    RX = bcm2835_spi_transfer(TX);
    //printf("%d. \n", ct);
    //clock_t start, end;
    //struct timeval tval_before, tval_after, tval_result;
    RX = bcm2835_spi_transfer(TX);
    if(RX != ACCEL) {
			//fprintf(stderr, "%d NEQ\n", ct);
			//printf("%x(%c) NEQ\n", RX, RX);
			//start =clock();
    }
    while(RX != ACCEL){
        op_err++;
				usleep(1000000);
				RX = bcm2835_spi_transfer(TX);
				printf("%x(%c) NEQ\n", RX, RX);
//				fprintf(stderr,"%x(%c)|", RX, RX);
    }
/*
   if(op_err > 0) {
      fprintf(stderr, "ct=%d \t errors:%d\n", ct, op_err);
      end = clock();
      float diffsec = (float)(end - start)/CLOCKS_PER_SEC;
      fprintf(stderr," ---- error time %.6f sec\n", diffsec);
      fprintf(stderr, "---------------------------");
   }
   printf("%x(%c) ", RX, RX);
*/   
 //  gettimeofday(&tval_before, NULL);
 //  start =clock();
		int j = 0;
   //for(j=0;j<100;j++){
		 printf("Bat: %d%% %f Data: ", bat1, (bat1+(float)bat2/256));
		for(i=0;i<12;i++){
	    RX = bcm2835_spi_transfer(TX);
	    printf("%x ", RX);	
    }
		printf("\n");
	 //}
	    RX = bcm2835_spi_transfer(TX);
   // end = clock();


  //  gettimeofday(&tval_after, NULL);
 //   timersub(&tval_after, &tval_before, &tval_result);
 //   printf(" ---- transfer time %ld.%06ld sec", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
 //   ct++;
 //   printf("\n");
		//if(packets%100==0)
	    usleep(200000);
  }
	seconds++;
	}
	float diffsec = (float)(clock() - total)/CLOCKS_PER_SEC;
  printf("\nERR %d|| total time = %f\n", op_err,diffsec);

  bcm2835_spi_end();
  bcm2835_close();

}
