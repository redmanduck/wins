#include <bcm2835.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>
using namespace std;

class ctalk
{
  public:
    ctalk(int spd);
    static const int Hz3 = 3;
};

const int ctalk::Hz3;

ctalk::ctalk(int clkspeed){
  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
}

int main(){

}
