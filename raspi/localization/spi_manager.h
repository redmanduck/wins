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
	  static void UpdateInputBuffer();

	 public:
	  static mutex buffer_mutex;
	  static string input_buffer;

	  static void Init();
	  static void ShutDown();
	};

}

#endif
