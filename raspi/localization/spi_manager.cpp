#include <stdexcept>
#include "spi_manager.h"

namespace wins {

	mutex SPI::buffer_mutex;
	string SPI::input_buffer;

	uint8_t transmit_buffer[1024];

	void SPI::Init() {

		  if (!bcm2835_init()) {
				throw runtime_error("Unable to init bcm2835!");
		  }
	  	  bcm2835_spi_begin();
		  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
		  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
		  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);   //4096); //2048);
		  bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
		  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
	}

	void SPI::ShutDown() {
	  // WINSTODO: Pic handshake
	}

}
