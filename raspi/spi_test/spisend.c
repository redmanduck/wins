/*
 *  Send data over SPI
 *
 */


#include <bcm2835.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>


int main(){
  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
 
    printf( "Sending ...\n");
    char mosi[10] = { 0xAB, 0xCD, 0xEF, 0xEE, 0x0 , 0x0, 0x0, 0x0, 0x00 };
    char miso[10] = { 0 };

    bcm2835_spi_transfernb(mosi, miso, 4);

    printf("RX: ");
 
    int i = 0;
    for(i = 0; i < 10; i++){
     printf("0x%x " ,miso[i]);
    } 
  
 
  printf("\n");

  bcm2835_spi_end();
  bcm2835_close();

}
