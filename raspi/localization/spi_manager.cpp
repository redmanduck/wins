#include <atomic>
#include <stdexcept>
#include <thread>

#include "global.h"
#include "imu.h"
#include "log.h"
#include "spi_manager.h"
#include "display.h"

namespace wins {

#define SPI_INTERVAL 2400
#define PACKET_SIZE 12

atomic_bool SPI::terminate_(false);
thread SPI::spi_thread_;

// SPI Transfer.
uint8_t SPI::Exchange(uint8_t send_byte) {
  if (not init_success_) {
    return OP_BUSY;
  }
}

// The Kenzhebalin Protocol.
void SPI::MainLoop() {
  while(not terminate_) {
    this_thread::sleep_for(chrono::microseconds(SPI_INTERVAL));

    if (next_packet_ == (int)ceil(1024 / PACKET_SIZE)) {
      next_packet_ = 0;
      auto& display = Display::GetInstance();
      lcd_buffer_ = display.GetBufferCopy();
    }

    // Try initiating packet transfer.
    uint8_t received = Exchange(OP_START);

    // PIC is busy :(
    if (received == OP_BUSY) {
      continue;
    } else if (received == OP_SHUTDOWN) {
      Global::SetEventFlag(WINS_EVENT_SHUTTING_DOWN);
      return;
    }

    // Exchange 1 packet.
    vector<uint8_t> pic_data;
    uint8_t send_checksum = 0;
    uint8_t receive_checksum = 0;
    uint8_t send_byte = 0;
    for (int i = 0; i < PACKET_SIZE; ++i) {
      if (next_packet_ * PACKET_SIZE + i < 1024) {
        send_byte = lcd_buffer_.get()[next_packet_ * PACKET_SIZE + i];
      } else {
        send_byte = 0;
      }
      received = Exchange(send_byte);
      pic_data.push_back(received);

      send_checksum += send_byte;
      receive_checksum += received;
    }

    // Exchange checksums.
    uint8_t pic_checksum = Exchange(send_checksum);
    if (receive_checksum != pic_checksum) {
      // WINSTODO: Find a way to fix this...
      FILE_LOG(logERROR) << "PIC Checksum error";
      continue;
    }

    // Add IMU reading onto buffer.
    Imu::AddReading(pic_data);
    next_packet_ += 1;
  }
}

SPI::SPI() {
  terminate_ = false;
  if (!bcm2835_init()) {
    FILE_LOG(logERROR) << "Unable to init bcm2835!";
    cout << "Unable to init bcm2835!";
    init_success_ = false;
    return;
  }
  bcm2835_spi_begin();
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, 0);
  bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, 0);
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024);   //4096); //2048);
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);
  bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
  init_success_ = true;
}

SPI& SPI::GetInstance() {
  static SPI spi;
  return spi;
}

void SPI::StartThread() {
  if (not spi_thread_.joinable()) {
    spi_thread_ = thread(&SPI::MainLoop, &SPI::GetInstance());
  }
}

void SPI::TerminateThread() {
  terminate_ = true;
  spi_thread_.join();
}

}
