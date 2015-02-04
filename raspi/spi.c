#include <bcm2835.h>
#include <stdio.h>

int main(){
  if (!bcm2835_init()) {
        printf("oops, could not init bcm2835\n");
        return 1;
  }
  
  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
  
  uint8_t mosi[10] = { 0x60, 0x10, 0xFF, 0xAA, 0x0A };
  uint8_t miso[10] = { 0 };

  bcm2835_spi_transfernb(mosi, miso, 10);

  int i = 0;
  for(i = 0; i < 10; i++){
    printf("0x%x " ,miso[i]);
  }

  bcm2835_spi_end();
  bcm2835_close();

}
