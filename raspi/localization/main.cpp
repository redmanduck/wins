#include <iostream>

#include "map.h"
#include "test.h"

using namespace std;

void RunMainLoop(int argc, char *argv[]) {
  while(not Global::ShuttingDown) {
    Location::UpdateEstimate();
    Navigation::UpdateRoute();
    Display::display_update_pending.notify_one();
  }
}

int main(int argc, char *argv[]) {
  if (string(argv[1]) == "test") {
    Test(argc, argv);
  } else {
    Global::Init();
    SPI::Start();
    RunMainLoop(argc, argv);
  }
  return 0;
}
