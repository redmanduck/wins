#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <cstdlib>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

using namespace std;

#define DSCAN

struct PointEstimate {
  float x_mean;
  float x_var;
  float y_mean;
  float y_var;
  string to_string() {
    return std::to_string(x_mean) + "," +
           std::to_string(x_var) + "," +
           std::to_string(y_mean) + "," +
           std::to_string(y_var);
  }
};

class Global {
 public:
  static string MapFile;

  static void Init();
};

template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::false_type, Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

template <typename T, typename... Args>
std::unique_ptr<T> make_unique_helper(std::true_type, Args&&... args) {
   static_assert(std::extent<T>::value == 0,
       "make_unique<T[N]>() is forbidden, please use make_unique<T[]>().");

   typedef typename std::remove_extent<T>::type U;
   return std::unique_ptr<T>(new U[sizeof...(Args)]{std::forward<Args>(args)...});
}

template <typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
   return make_unique_helper<T>(std::is_array<T>(), std::forward<Args>(args)...);
}

#endif // COMMON_UTILS_H
