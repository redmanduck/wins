#include <cstdlib>

#include "common_utils.h"
#include "display.h"
#include "spi_manager.h"

string Global::MapFile = "ee_ab_map.dat";
Display* Global::display_;
int Global::FilterableDistance = 10;
int Global::FilterBiasX = 5;
int Global::FilterBiasY = 5;

void Global::Init() {
  SPI::Init();
  display_ = new Display();
}

void Global::Destroy() {
  delete display_;
}

Display* Global::MainDisplay() {
  return display_;
}

void Global::ShutDown() {
  Global::Destroy();
  SPI::ShutDown();
  system("sudo shutdown -P now");
  exit(0);
}
