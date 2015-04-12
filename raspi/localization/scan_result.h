#ifndef SCAN_RESULT_H
#define SCAN_RESULT_H

#include <string>

#include "cereal/cereal.hpp"
#include "cereal/types/string.hpp"

namespace wins {

using namespace std;

struct Result {
  string name;
  double signal;

  template<class Archive>
  void serialize(Archive & archive) {
    archive(name, signal);
  }
};

}

#endif //SCAN_RESULT_H
