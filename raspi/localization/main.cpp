#include <iostream>
#include <cstring>

#include "display.h"
#include "global.h"
#include "location.h"
#include "navigation.h"
#include "test.h"

int main(int argc, char *argv[]) {
  using namespace wins;
  using namespace std;

  if (string(argv[1]) == "test") {
    Test(argc, argv);
  } else {
    if (argc < 2) {
      cout << "Map filename required";
      return 1;
    }
    Global::MapFile = string(argv[1]);
    Global::RunMainLoop();
  }
  return 0;
}
