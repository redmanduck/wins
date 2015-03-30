#ifndef COMMON_UTILS_H
#define COMMON_UTILS_H

#include <algorithm>
#include <atomic>
#include <cstdlib>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <cassert>

class Display;

using namespace std;

#define DSCAN

struct PointEstimate {
  double x_mean;
  double x_var;
  double y_mean;
  double y_var;
  string to_string() {
    return std::to_string(x_mean) + "," +
           std::to_string(x_var) + "," +
           std::to_string(y_mean) + "," +
           std::to_string(y_var);
  }
};

class Global {
 private:
  static Display* display_;

 public:
  static int FilterableDistance;
  static int FilterBiasX;
  static int FilterBiasY;
  static string MapFile;

  static void Init();
  static void Destroy();
  static Display* MainDisplay();
  static void ShutDown();
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

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::plus<T>());
    return result;
}

template <typename T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::minus<T>());
    return result;
}

template <typename T>
std::vector<T> operator*(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::multiplies<T>());
    return result;
}

template <typename T>
std::vector<T> operator/(const std::vector<T>& a, const std::vector<T>& b)
{
    assert(a.size() == b.size());

    std::vector<T> result;
    result.reserve(a.size());

    std::transform(a.begin(), a.end(), b.begin(),
                   std::back_inserter(result), std::divides<T>());
    return result;
}

template <int N>
std::vector<int> ones() {
  return std::vector<int>(1, N);
}

template <int N>
std::vector<int> zeroes() {
  return std::vector<int>(0, N);
}

#endif // COMMON_UTILS_H
