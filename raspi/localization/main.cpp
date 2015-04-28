#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sys/stat.h>

#include "display.h"
#include "global.h"
#include "location.h"
#include "navigation.h"
#include "test.h"

const char DEBUG_FILE[] = "/wins/raspi/localization/DEBUG";

int main(int argc, char *argv[]) {
  using namespace wins;
  using namespace std;

  struct stat buffer;
  if (stat((string(getenv("HOME")) + DEBUG_FILE).c_str(), &buffer) == 0) {
    cout << "Setting test mode...\n";
    Global::SetTestMode();
  }

  if (argc >= 2 and string(argv[1]) == "test") {
    Test(argc, argv);
  } else {
    if (argc == 2) {
      Global::MapFile = string(argv[1]);
    }
    Global::RunMainLoop();
  }
  return 0;
}
