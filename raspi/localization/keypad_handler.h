#ifndef KEYPAD_HANDLER_H
#define KEYPAD_HANDLER_H

#include <string>
#include <mutex>
#include <thread>

namespace wins {

using namespace std;

class KeypadHandler{
 private:
  static thread keypad_thread_;

  mutex buffer_mutex_;
  string buffer_;

  void ProcessButton(int row, int col);
  void MainLoop();
	KeypadHandler();

  KeypadHandler(KeypadHandler const&)  = delete;
  void operator=(KeypadHandler const&) = delete;

 protected:
  int GetEvent();

 public:
  static KeypadHandler& GetInstance();
  static void StartThread();
  static void TerminateThread();

  char GetChar();
};

}

#endif
