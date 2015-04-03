#ifndef SHUTDOWN_EXCEPTION_H
#define SHUTDOWN_EXCEPTION_H

#include <stdexcept>

namespace {

class ShutDownException : public runtime_error {
 public:
  ShutDownException() : runtime_error("System Shutting Down") {}
};

}

#endif
