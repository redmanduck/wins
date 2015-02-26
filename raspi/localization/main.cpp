#include <iostream>

#include "map.h"
#include "test.h"

using namespace std;

void RunMainLoop(int argc, char *argv[]) {

}

int main(int argc, char *argv[]) {
  if (string(argv[1]) == "test") {
    Test(argc, argv);
  } else {
    RunMainLoop(argc, argv);
  }
  return 0;
}
