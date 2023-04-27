#include <iostream>
#include <memory>
template <class Derived>
class Base {
 public:
  void name() { (static_cast<Derived*>(this))->impl(); }
};

class D1 : public Base<D1> {
 public:
  void impl() { std::puts("D1::impl()"); }
};
class D2 : public Base<D2> {
 public:
  void impl() { std::puts("D2::impl()"); }
};
// reference version
template <class Derived>
void handle(Base<Derived>& dd) {
  dd.name();
}

// pointer version
template <typename Derived>
void handle2(std::shared_ptr<Base<Derived>>& dd) {
  dd->name();
}

// using example
void test() {
  // Base<D1> b1; b1.name(); //undefined behavior
  // Base<D2> b2; b2.name(); //undefined behavior
  D1 d1;
  d1.name();
  D2 d2;
  d2.name();

  handle(d1);

  std::shared_ptr<Base<D1>> d11 = std::make_shared<D1>();
  std::shared_ptr<Base<D2>> d22 = std::make_shared<D2>();
  handle2(d11);
  handle2(d22);
}

int main() { test(); }
