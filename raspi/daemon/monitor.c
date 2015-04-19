#define WINSD_VER "monitor_2.0"
#define BUF_SIZE 1024
#define CHUNK_SIZE 1024
#define TIME_BETWEEN_CHUNK 0
#define TIME_BETWEEN_BUF 14000
#include <time.h>

#include <bcm2835.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/time.h>

#include "animation.h"

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
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2048);   //4096); //2048);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);

  int var = 0;
	char imu_buf[BUF_SIZE];
	for(var = 0; var < BUF_SIZE; var++){
		imu_buf[var]=0x00;
	}
	//char lcd_buf[BUF_SIZE] = {
		char * lcd_buf = getBitmap();
		/*
		0x00, 0x00,0x00, 0x00, 
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x42,
		0xFE,0x02,0x00,0x00,0x00,0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0xFE,0x20,0x10,0x08,0xFE,0x00,0x3E,0x48,0x88,0x48,0x3E,0x00,0xF8,0x04,0x02,0x04,
		0xF8,0x00,0x00,0x82,0xFE,0x82,0x00,0x00,0x7C,0x82,0x82,0x8A,0xCE,0x00,0x3E,0x48,
		0x88,0x48,0x3E,0x00,0xC0,0x80,0xFE,0x80,0xC0,0x00,0x00,0x82,0xFE,0x82,0x00,0x00,
		0x7C,0x82,0x82,0x82,0x7C,0x00,0xFE,0x20,0x10,0x08,0xFE,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4E,0x92,
		0x92,0x92,0x62,0x00,0x00,0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0xFC,0x02,0x1C,0x02,0xFC,0x00,0xFE,0x10,0x10,0x10,0xFE,0x00,0xFE,0x92,0x92,0x92,
		0x82,0x00,0xFE,0x90,0x98,0x94,0x62,0x00,0xFE,0x92,0x92,0x92,0x82,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x3E,0x48,0x88,0x48,0x3E,0x00,0xFE,0x40,0x38,0x40,0xFE,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x82,0xFE,0x82,0x00,0x00,0x40,0x80,0x9A,0x90,
		0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x84,0x82,
		0x92,0xB2,0xCC,0x00,0x00,0x00,0x06,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x64,0x92,0x92,0x92,0x4C,0x00,0xFE,0x10,0x10,0x10,0xFE,0x00,0xFC,0x02,0x02,0x02,
		0xFC,0x00,0xC0,0x80,0xFE,0x80,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x82,
		0x82,0x82,0x7C,0x00,0x7C,0x82,0x82,0x82,0x7C,0x00,0xFC,0x02,0x1C,0x02,0xFC,0x00,
		0xFE,0x20,0x10,0x08,0xFE,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
  */
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
	int data_p = 0;
	printf("Begin\n");
	while(packets<265){
		// Establishing communication for getting battery data
		TX = BAT;
    RX = bcm2835_spi_transfer(TX);
    RX = bcm2835_spi_transfer(TX);
    while(RX != BAT){
        op_err++;
				usleep(10000);
				RX = bcm2835_spi_transfer(TX);
				printf("| tx:%x rx:%x(%c) |", TX, RX, RX);
				if(op_err%10==0)
					printf("\n");
    }
    RX = bcm2835_spi_transfer(TX);
		bat1 = RX;
    RX = bcm2835_spi_transfer(TX);
	bat2 = RX;
	// battery status is stored in bat1 and bat2
	data_p=0;

	//trasnferring 1024 bytes of data: 12 at a time
  while(data_p < BUF_SIZE){
    TX = ACCEL;
//printf("%d\n", packets);
    RX = bcm2835_spi_transfer(TX);
    //printf("%d. \n", ct);
    //clock_t start, end;
    //struct timeval tval_before, tval_after, tval_result;
    RX = bcm2835_spi_transfer(TX);
    while(RX != ACCEL){
        op_err++;
	usleep(10000);
	RX = bcm2835_spi_transfer(TX);
	printf("| tx:%x rx:%x(%c) |", TX, RX, RX);
	if(op_err%10==0)
	printf("\n");
	//	fprintf(stderr,"%x(%c)|", RX, RX);
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
	printf("%d:Bat: %d%% %f Data: ", data_p, bat1, (bat1+(float)bat2/256));
	for(i=0;i<CHUNK_SIZE;i++){
		TX=lcd_buf[data_p+i];
		RX = bcm2835_spi_transfer(TX);
		imu_buf[data_p+i]=RX;
		printf("%x ", RX);	
	}
		int x = (imu_buf[data_p+0]<<8) | imu_buf[data_p+1];
		int y = (imu_buf[data_p+2]<<8) | imu_buf[data_p+3];
		int z = (imu_buf[data_p+4]<<8) | imu_buf[data_p+5];
		int gx = (imu_buf[data_p+6]<<8) | imu_buf[data_p+7];
		int gy = (imu_buf[data_p+8]<<8) | imu_buf[data_p+9];
		int gz = (imu_buf[data_p+10]<<8) | imu_buf[data_p+11];
		printf("\nax:%d y:%d z:%d. gx:%d y:%d z:%d\n",x,y,z,gx,gy,gz);
		RX = bcm2835_spi_transfer(TX);
		if(RX == VALID){
			data_p+=CHUNK_SIZE;
		}
   // end = clock();


  //  gettimeofday(&tval_after, NULL);
 //   timersub(&tval_after, &tval_before, &tval_result);
 //   printf(" ---- transfer time %ld.%06ld sec", (long int)tval_result.tv_sec, (long int)tval_result.tv_usec);
 //   ct++;
 //   printf("\n");
		//if(packets%100==0)
	    usleep(TIME_BETWEEN_CHUNK);
		}
		printf("PACKET %d transfered\n", packets);
		packets++;
		lcd_buf = getBitmap();
		usleep(TIME_BETWEEN_BUF);
	}
	float diffsec = (float)(clock() - total)/CLOCKS_PER_SEC;
  printf("\nERR %d|| total time = %f\n", op_err,diffsec);

  bcm2835_spi_end();
  bcm2835_close();

}
