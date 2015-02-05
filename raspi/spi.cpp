#include <bcm2835.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
using namespace std;

int main(){
  if (!bcm2835_init()) {
        cout << "oops!";
        return 1;
  }
  
  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
  
  char mosi[10] = { 0xFF, 0xAB, 0xAC, 0xCC, 0x0 , 0x0, 0x0, 0x0, 0x00 };
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
