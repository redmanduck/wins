#ifndef SPI_MANAGER_H
#define SPI_MANAGER_H

#include <mutex>

#include "common_utils.h"

namespace wins {

class SPI {
 private:
  static void UpdateInputBuffer();

 public:
  static mutex buffer_mutex;
  static string input_buffer;

  static void Init();
  static void ShowMenu();
  static void ShutDown();
};

}

#endif
