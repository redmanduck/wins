#include <stdexcept>

#include "spi_manager.h"

namespace wins {

mutex SPI::buffer_mutex;
string SPI::input_buffer;

void SPI::Init() {
  // TODO: Pic handshake
}

void SPI::ShowMenu() {
  throw runtime_error("Not Implemented");
}

void SPI::ShutDown() {
  throw runtime_error("Not Implemented");
}

}
