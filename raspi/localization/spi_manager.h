#ifndef SPI_MANAGER_H
#define SPI_MANAGER_H

#include <mutex>

#include "common_utils.h"
#include <bcm2835.h>

namespace wins {

#define BUF_SIZE 896

enum opcode {
    IMU = 'M',
    ACCEL = 'A',
    GYRO = 'G',
    KEYPAD = 'K',
    HALT = 'H',
    ULTRASONIC = 'U',
    POSITION = 'P',
    VALID = 'V',
    ERROR = 'E',
    BAT = 'B',
};

class SPI {
 private:
  static atomic_bool terminate_;
  static thread spi_thread_;
  int next_packet_;
  bool init_success_;

  uint8_t imu_buf[BUF_SIZE];
  unique_ptr<uint8_t> lcd_buffer_;
  char RX = '0';
  char TX = '0';
  int i = 0;
  int ct = 0;
  int err = 0;
  int op_err = 0;
  int packets = 0;
  int seconds = 0;
  unsigned char bat1 = 0;
  unsigned char bat2 = 0;
  int data_p = 0;

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
