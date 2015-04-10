#include <stdexcept>

#include "spi_manager.h"

namespace wins {

mutex SPI::buffer_mutex;
string SPI::input_buffer;

void SPI::Init() {
  // WINSTODO: Pic handshake
}

void SPI::Transfer() {
  //

void SPI::ShutDown() {
  // WINSTODO: Pic handshake
}

}
