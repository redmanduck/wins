#include <iostream>

#include "display.h"
#include "global.h"
#include "location.h"
#include "navigation.h"
#include "test.h"

int main(int argc, char *argv[]) {
  using namespace wins;

  if (string(argv[1]) == "test") {
    Test(argc, argv);
  } else {
    Global::Init();
    Global::RunMainLoop();
  }
  return 0;
}
