#ifndef SPI_MANAGER_H
#define SPI_MANAGER_H

#include <mutex>

#include "common_utils.h"
#include <bcm2835.h>

namespace wins {

enum opcode {
    OP_IMU,
    OP_ACCEL,
    OP_GYRO,
    OP_KEYPAD,
    OP_HALT,
    OP_ULTRASONIC,
    OP_POSITION,
    OP_VALID,
    OP_ERROR,
    OP_BAT,
    OP_BUSY,
    OP_START,
    OP_SHUTDOWN
};

class SPI {
 private:
  static atomic_bool terminate_;
  static thread spi_thread_;
  int next_packet_;
  unique_ptr<uint8_t> lcd_buffer_;

  uint8_t Exchange(uint8_t send_byte);
  void MainLoop();
  SPI();

  SPI(SPI const&) = delete;
  void operator=(SPI const&) = delete;

 public:
  static SPI& GetInstance();
  static void StartThread();
  static void TerminateThread();
};

}

#endif
