#include "pibridge.h"
#include "bcm2835.h"
#include <iostream>

pibridge::pibridge() {
  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_2048);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
}

IMUdata * pibridge::recv_IMU(){

  uint8_t mosi[12] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
  uint8_t miso[12] = { 0 };

  //Transmit opcode
  for(int i = 0; i<2; i++){
      char response = bcm2835_spi_transfer(IMU);
      if(response != IMU){
          //fails
          return NULL;
      }
  }
  
  //start receiving data
  for(int i = 0; i < 12; i++){
      miso[i]=bcm2835_spi_transfer(mosi[i]);
  }
  
  for(int i = 0; i < 12; i++){
      std::cout << miso[i] << "\n";
  }
}

int main(){
    pibridge * PB = new pibridge();
    while(1){
        PB->recv_IMU();
    }
}