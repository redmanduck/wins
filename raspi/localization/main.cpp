#include <iostream>

#include "display.h"
#include "location.h"
#include "map.h"
#include "navigation.h"
#include "test.h"

using namespace std;

void RunMainLoop(int argc, char *argv[]) {
  while(true) {
    if (not Map::IsNavigating()) {
      Map::BlockUntilNavigating();
    }
    Location::UpdateEstimate();
    Navigation::UpdateRoute();
    Global::MainDisplay()->SetUpdateFlag(DISPLAY_UPDATE_NAV_CHANGE);
  }
}

int main(int argc, char *argv[]) {
  if (string(argv[1]) == "test") {
    Test(argc, argv);
  } else {
    Global::Init();
    RunMainLoop(argc, argv);
  }
  return 0;
}
