#include <iostream>

#include "display.h"
#include "global.h"
#include "location.h"
#include "navigation.h"
#include "test.h"

namespace wins {

using namespace std;

void RunMainLoop() {
  Display& display = Display::GetInstance();
  Page curpage;
  Page nextpage = display.FirstPage();

  while(nextpage != PAGE_SHUT_DOWN) {
    // Just to be clear.
    curpage = nextpage;

    nextpage = display.ShowPage(curpage);
  }
  display.ShowPage(PAGE_SHUT_DOWN);
  Global::ShutDown();
}

}

int main(int argc, char *argv[]) {
  using namespace wins;

  if (string(argv[1]) == "test") {
    Test(argc, argv);
  } else {
    Global::Init(argc, argv);
    RunMainLoop();
  }
  return 0;
}
