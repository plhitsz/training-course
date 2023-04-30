#include <iostream>
#include <memory>
#include <variant>
template <class Derived>
class Base {
 public:
  std::string& name() { return (static_cast<Derived*>(this))->impl(); }
};

class D1 : public Base<D1> {
 public:
  std::string& impl() { return msg_; }

 private:
  std::string msg_ = {"D2::impl()"};
};
class D2 : public Base<D2> {
 public:
  std::string& impl() { return msg_; }

 private:
  std::string msg_ = {"D2::impl()"};
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

// set pointer type of CRTP class
class Test {
 public:
  // save ptr to handler_
  template <typename Derived>
  void setHandler(std::shared_ptr<Base<Derived>> dd) {
    handler_ = dd;
  }
  // get name by reference
  std::string& getName() {
    return std::visit(
        [](auto& handler) -> std::string& { return handler->name(); },
        handler_);
  }

  template <typename Derived>
  void setHandler2(std::shared_ptr<Base<Derived>> dd) {
    if constexpr (std::is_same_v<Derived, D1>) {
      h1_ = dd;
    } else if constexpr (std::is_same_v<Derived, D2>) {
      h2_ = dd;
    }
  }
  std::string& getName2() {
    if (h1_ != nullptr) {
      return h1_->name();
    } else if (h2_ != nullptr) {
      return h2_->name();
    }
  }
  std::variant<std::shared_ptr<Base<D1>>, std::shared_ptr<Base<D2>>> handler_;
  std::shared_ptr<Base<D1>> h1_;
  std::shared_ptr<Base<D2>> h2_;
};

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

  Test tt;
  std::shared_ptr<Base<D1>> d1_ptr = std::make_shared<D1>();
  tt.setHandler(d1_ptr);
  std::cout << tt.getName() << std::endl;
}

int main() { test(); }
