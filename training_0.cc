/**
 * @file training_0.cc
 * @author Lei Peng
 * @brief n-hop training course: c++11 to c++17, brief introduction of the
 * features. This source code will use live example to show the new features of
 * modern c++.
 * @version 0.1
 * @date 2023-04-22
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <algorithm>
#include <any>
#include <cassert>
#include <chrono>
#include <functional>
#include <future>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>
/// @memory order
/// "Absent any constraints on a multi-core system, when multiple threads
/// simultaneously read and write to several variables, one thread can observe
/// the values change in an order different from the order another thread wrote
/// them."
/// reference: https://en.cppreference.com/w/cpp/atomic/memory_order
#include <atomic>

// relaxed: std::memory_order_relaxed
void feat_0() {
  std::atomic<int> r1 = {0};
  std::atomic<int> y = {0};
  std::atomic<int> x = {0};
  std::atomic<int> r2 = {0};
  std::vector<std::thread> v;
  // guess the order of A, B, C, D
  v.emplace_back([&x, &y, &r1]() {
    // Thread 1:
    r1 = y.load(std::memory_order_relaxed);  // A(读y)
    x.store(r1, std::memory_order_relaxed);  // B(写x)
  });

  v.emplace_back([&x, &y, &r2]() {
    // Thread 2:
    r2 = x.load(std::memory_order_relaxed);  // C(读x)
    y.store(42, std::memory_order_relaxed);  // D(写y)
  });
  for (auto& t : v) {
    t.join();
  }
  std::cout << "Final x = " << x << ", y = " << y << ", r1 = " << r1
            << ", r2 = " << r2 << std::endl;
  // Final x = 42, y = 42, r1 = 42, r2 = 0:
  // Final x = 0, y = 42, r1 = 0, r2 = 0:
}
// Usage: the count of shared_ptr
void feat_1() {
  // relaxed: They only guarantee atomicity and modification order consistency.
  std::atomic<int> cnt = {0};
  std::vector<std::thread> v;
  for (int n = 0; n < 10; ++n) {
    v.emplace_back([&cnt]() {
      for (int n = 0; n < 1000; ++n) {
        cnt.fetch_add(1, std::memory_order_relaxed);
      }
    });
  }
  for (auto& t : v) {
    t.join();
  }
  std::cout << "Final counter value is " << cnt << std::endl;
}
// store-Release : load-acquire
// "All memory writes (including non-atomic and relaxed atomic) that
// happened-before the atomic store from the point of view of thread A, become
// visible side-effects in thread B."
void feat_2() {
  std::atomic<int> xx = {0};
  std::atomic<int> ret = {0};
  std::vector<std::thread> v;
  // A thread
  v.emplace_back([&xx, &ret]() {
    // A(写)
    ret = 99;
    xx.store(1, std::memory_order_release);
  });
  // B thread
  v.emplace_back([&xx, &ret]() {
    // whenever B finish load, A(写) is visible to B
    // This promise only holds if B actually returns the value that A stored, or
    // a value from later in the release sequence.
    while (xx.load(std::memory_order_acquire) == 0)
      ;
    // B(读)
    assert(ret == 99);
  });
  for (auto& t : v) {
    t.join();
  }
}

/// auto & decltype
template <typename T>
void f(T& r) {
  auto v = r;          // v 是 T
  decltype(r) r2 = r;  // r2 是 T&
}
// todo: example of why we need decltype or auto.
// pretty format
auto sum(int a, int b) -> int { return a + b; }
auto create_vec() -> std::vector<int> { return std::vector<int>(); }
auto sum2(int a, int b) -> decltype(a + b) { return a + b; }
// refactoring: change the return type of sum
void refactoring() { auto res = sum(1, 2); }

/// for ...
void feat_3() {
  int sum = 0;
  // initializer_list
  for (auto i : {1, 2, 3, 5, 8}) sum += i;

  std::vector<std::string> str_vec = {"hello", "world"};
  for (const auto& str : str_vec) {
    std::cout << str << std::endl;
  }
}

/// move
class Test {
 public:
  Test() {}
  ~Test() {}
  Test(const Test&) { std::cout << "copy construct" << std::endl; }
  Test& operator=(const Test&) {
    std::cout << "copy assignment" << std::endl;
    return *this;
  }
  // move constructor and move assignment
  Test(Test&&) noexcept { std::cout << "move construct" << std::endl; }
  Test& operator=(Test&&) noexcept {
    std::cout << "move assignment" << std::endl;
    return *this;
  }
  int i = 0;
};

void feat_4() {
  Test t;
  // move construct
  Test t2 = std::move(t);  //&&
  // move assignment
  Test t3;
  t3 = std::move(t);

  // move of STL container
  std::string str = "hello";
  auto new_str = std::move(str);
}

/// rvalue reference
void feat_5() {
  // rvalue reference
  int&& r = 1;
  // lvalue reference
  int& l = r;

  // why we need rvalue reference
  Test t;
  // move construct
  Test t2 = std::move(t);
}

/// example of Uniform Initialization
// std::initializer_list<T>
class myVec {
 public:
  myVec(const std::initializer_list<int>& list) {
    for (auto& i : list) {
      std::cout << i << ' ';
    }
    std::cout << std::endl;
  }
};

void feat_6() {
  std::vector<int> vvvv(10);
  std::vector<int> vvvvv(10, 1);
  std::vector<int> v = {10};
  std::vector<int> vv = {1, 2, 3};

  myVec vec = {1, 2, 3, 4, 5};
  myVec vec2({1, 2, 3, 4, 5});
}

/// lambda
void feat_7() {
  auto sum_f = [](int a, int b) { return a + b; };
  sum_f(10, 9);

  // capture
  int a = 10;
  auto sum_f2 = [&a](int b) { return a + b; };
  sum_f2(9);

  // generic lambda: auto can be deduced.
  auto sum_f3 = [](auto a, auto b) { return a + b; };

  // use for call back
  std::vector<int> v{3, -4, 2, -8, 15, 267};
  auto print = [](const int& n) { std::cout << n << ' '; };
  std::for_each(v.cbegin(), v.cend(), print);

  std::vector<int> v1{1, 2, 3};
  std::vector<std::string> v2{"a", "b", "c"};
  std::for_each(v1.cbegin(), v1.cend(),
                [](const auto& n) { std::cout << n << ' '; });
  std::for_each(v2.cbegin(), v2.cend(),
                [](const auto& n) { std::cout << n << ' '; });

  std::cout << std::endl;
  // function of function
  auto print_less_than = [](int x) {
    return [x](auto y) {
      if (y < x) {
        std::cout << y << ' ';
      }
    };
  };
  std::for_each(v.cbegin(), v.cend(), print_less_than(15));
  std::cout << std::endl;
  std::cout << "bind version:" << std::endl;
  // use std::bind to implement print_less_than
  auto less_than = [](int x, auto y) {
    if (y < x) {
      std::cout << y << ' ';
    }
  };
  std::for_each(v.cbegin(), v.cend(),
                std::bind(less_than, 15, std::placeholders::_1));
  std::cout << std::endl;

  // more bind example
  int n = 7;
  auto f = [](int n1, int n2, int n3, const int& n4, int n5) {
    std::cout << n1 << ' ' << n2 << ' ' << n3 << ' ' << n4 << ' ' << n5 << '\n';
  };
  auto f1 = std::bind(f, std::placeholders::_2, 42, std::placeholders::_1,
                      std::cref(n), n);
  n = 10;
  f1(1, 2, 1001);  // 1 is bound by _1, 2 is bound by _2, 1001 is unused
                   // makes a call to f(2, 42, 1, n, 7)

  std::cout << "move capture:" << std::endl;
  // move capture
  auto res = std::make_unique<Test>();
  auto handle_test = [move_res = std::move(res)]() mutable {
    std::cout << move_res->i << std::endl;
  };
  handle_test();
}

/// template
/*
class ThreadPool {
 public:
  /// @brief execute a task by using thread pool
  template <typename F, typename... Args>
  bool ExecuteTask(F&& f, Args&&... args) {
    // packaged_task
    auto task = std::make_shared<std::packaged_task<bool()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    // ...
    return true;
  }
};

void feat_8() {
  ThreadPool pool;
  pool.ExecuteTask([](int a, int b) -> bool {
    std::cout << "hello" << std::endl;
    return true;
  });

  pool.ExecuteTask([](int a, int b, int c) -> bool {
    std::cout << "hello" << std::endl;
    return true;
  });
}
*/

// handler(RAII) -> smart pointer -> GC
class Resource {
 public:
  Resource() { str = new std::string("hello"); }
  ~Resource() { delete str; }
  virtual void doSomething() { std::cout << *str << std::endl; }
  std::string* str = nullptr;
};

void feat_9() {
  Resource r;
  r.doSomething();
  // unique_ptr
  auto res = std::make_unique<Resource>();
  res->doSomething();
  auto res_copy = std::move(res);
  res_copy->doSomething();

  // share_ptr
  auto res2 = std::make_shared<Resource>();
  res2->doSomething();
  auto res2_copy = res2;
  res2_copy->doSomething();

  // weak_ptr to break the cycle
  std::weak_ptr<Resource> res_weak = res2;
  if (!res_weak.expired()) {
    res_weak.lock()->doSomething();
  } else {
    throw std::runtime_error("Resource is expired!");
  }
}

/// override and final
class DerivedResource : public Resource {
 public:
  void doSomething() override final { std::cout << "hello!" << std::endl; }
};

void feat_10() {
  DerivedResource d;
  d.doSomething();
}

/// enum class
enum class Color { green, yellow, red };
enum class TrafficLight { green, yellow, red };
void feat_11() {
  Color col = Color::green;
  TrafficLight light = TrafficLight::green;
}

/// constexpr
constexpr double _1k = 2 << 10;
constexpr int factorial(int n) { return n <= 1 ? 1 : (n * factorial(n - 1)); }
constexpr int n = factorial(5);

template <typename T>
std::string convertToString(T t) {
  if constexpr (std::is_same_v<T, std::string>)
    return t;
  else
    return std::to_string(t);
}

// another implementation : enable_if SFINAE
template <typename T>
std::enable_if_t<std::is_same_v<T, std::string>, std::string> convertToString(
    T t) {
  return t;
}
template <typename T>
std::enable_if_t<!std::is_same_v<T, std::string>, std::string> convertToString(
    T t) {
  return std::to_string(t);
}

// type trait
// https://en.cppreference.com/w/cpp/header/type_traits
// is_arithmetic
void feat_11_1() {
  std::cout << std::boolalpha << std::is_arithmetic_v<int> << std::endl;
  std::cout << std::boolalpha << std::is_arithmetic_v<std::string> << std::endl;
}
template <typename T>
class Matrix1 {
 public:
  Matrix1() {
    static_assert(std::is_arithmetic_v<T>,
                  "Matrix<T>: T must be arithmetic type");
  }
};
/*
Defined in header <type_traits>
template< bool B, class T = void >
struct enable_if;
If B is true, std::enable_if has a public member typedef type, equal to T;
otherwise, there is no member typedef.
*/
template <typename T,
          typename std::enable_if<std::is_arithmetic<T>::value, bool>::type = 0>
class Matrix2 {
 public:
  Matrix2() = default;
};

// stack
using intVec3 = std::array<int, 3>;
using intMatrix = std::array<std::array<int, 3>, 3>;
void feat_11_2() {
  intMatrix my_mat;
  std::array<int, 3> a = {1, 2, 3};
  for (auto& row : my_mat) {
    row = a;
  }
  std::for_each(my_mat.cbegin(), my_mat.cend(), [](const auto& row) {
    std::for_each(row.cbegin(), row.cend(),
                  [](const auto& col) { std::cout << col << ' '; });
    std::cout << std::endl;
  });
  // is not allowed
  // int n;
  // std::array<int, n> a;
}

/// emplace
void feat_12() {
  std::vector<std::string> v;
  v.emplace_back("hello");
  v.emplace_back("world");
  v.emplace_back("!");
}

/// std::any
void feat_13() {
  std::any a = 1;
  std::cout << std::any_cast<int>(a) << std::endl;
  a = 1.1;
  std::cout << std::any_cast<double>(a) << std::endl;
  a = std::string("hello");
  std::cout << std::any_cast<std::string>(a) << std::endl;
}

/// std::optional
/// return a valid name and may be empty
std::optional<std::string> get_name() {
  std::string name = "halo";
  if (false) {
    return std::nullopt;  // "failed" or -1
  }
  return name;
}

std::tuple<bool, std::string> get_name2() {
  std::string name = "hi";
  if (name.empty()) {
    return {false, name};
  }
  return {true, name};
}

void feat_14() {
  auto name = get_name();
  if (name.has_value()) {
    std::cout << name.value() << std::endl;
  }
  auto [valid, name2] = get_name2();
  if (valid) {
    std::cout << name2 << std::endl;
  }
}

/// 结构化绑定
std::tuple<int, int, int> get_param() { return {1, 2, 3}; }
void feat_15() {
  int a, b, c;
  auto res = get_param();
  a = std::get<0>(res);
  b = std::get<1>(res);
  c = std::get<2>(res);
  std::cout << a << ' ' << b << ' ' << c << std::endl;

  std::tie(a, b, c) = get_param();
  std::cout << a << ' ' << b << ' ' << c << std::endl;

  // 结构化绑定
  auto [aa, bb, cc] = get_param();
  std::cout << aa << ' ' << bb << ' ' << cc << std::endl;
}

// NULL 0 nullptr
/*
#ifndef __cplusplus
  #define NULL ((void*)0)
#else
  #define NULL 0
#endif
*/
void bar(int a, int* b) {}
void bar(int a, int i) {}
void feat_16() {
  int* p = NULL;
  std::cout << p << std::endl;
}

int main() {
  feat_7();
  // mutex
  // condition_variable
  // scoped_lock

  // thread
  /*
  std::thread t([]() {
    while (true) {
      std::cout << "hello" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });
  t.join();
  */
  // chrono  // ns
  uint64_t value = static_cast<uint64_t>(
      std::chrono::high_resolution_clock::now().time_since_epoch().count());
  auto value_us = static_cast<uint64_t>(value / 1000);
  auto value_ms = static_cast<uint64_t>(value / 1000000L);
  auto value_s = static_cast<uint64_t>(value / 1000000000L);

  using namespace std::literals;  // enables the usage of 1ms
  auto start = std::chrono::system_clock::now();
  std::cout << " time escaped "
            << (std::chrono::system_clock::now() - start) / 1ms << " ms"
            << std::endl;

  // future 和 packaged_task
  auto computation = [](int a, int b) -> bool {
    std::cout << "do some computation " << b + a << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return true;
  };

  auto task =
      std::make_shared<std::packaged_task<int(int, int)>>([](int a, int b) {
        std::cout << "do some computation " << b + a << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return a + b;
      });
  // ()
  (*task)(1, 3);

  // get `std::future` object and wait for results here.
  std::cout << task->get_future().get() << std::endl;

  // std::promise ---> std::future
  std::promise<int> producer;
  std::future<int> consumer = producer.get_future();
  // Note that the std::promise object is meant to be used only once.
  std::thread t1([&consumer]() {
    // std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "consumer get " << consumer.get() << std::endl;
  });
  std::thread t2([&producer]() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    producer.set_value(1);
  });
  t1.join();
  t2.join();

  // std::async
  std::cout << "async ======= " << std::endl;
  std::future<std::string> a3;
  {
    a3 = std::async(std::launch::async, []() -> std::string {
      std::this_thread::sleep_for(std::chrono::seconds(2));
      std::cout << "work done!" << std::endl;
      return "ok";
    });
    std::cout << "wait for result inside" << std::endl;
    // std::cout << a3.get() << std::endl;
  }
  std::cout << "wait for result outside" << std::endl;
  // std::cout << a3.get() << std::endl;
}
