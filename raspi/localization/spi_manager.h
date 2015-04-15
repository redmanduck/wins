#ifndef SPI_MANAGER_H
#define SPI_MANAGER_H

#include <mutex>

#include "common_utils.h"
#include <bcm2835.h>

namespace wins {

	enum opcode {
	    OP_IMU = 'M',
	    OP_ACCEL = 'A',
	    OP_GYRO = 'G',
	    OP_KEYPAD = 'K',
	    OP_HALT = 'H',
	    OP_ULTRASONIC='U',
	    OP_POSITION='P',
	    OP_VALID = 'V',
	    OP_ERROR = 'E',
	    OP_BAT = 'B'
	};

	class SPI {
	 private:
	  void UpdateInputBuffer();
	  void Transmit();

	 public:
	  mutex buffer_mutex;
	  string input_buffer;

	  void Init();
	  void ShutDown();

  	  static SPI& GetInstance();
	};

}

#endif
