/// template function in CRTP class.

#include <iostream>
#include <memory>

enum class SourceType {
  PROVIDER = 10,
  CALCULATOR = 11,
};

template <class Derived>
class ITest {
 public:
  ITest() {}
  virtual ~ITest() {}
  template <SourceType type>
  void Debug(int id) {
    static_cast<Derived*>(this)->template impl_Debug<type>(id);
  }
};

class Test : public ITest<Test> {
 public:
  Test() = default;
  ~Test() = default;
  template <SourceType type>
  void impl_Debug(int id) {
    if constexpr (type == SourceType::PROVIDER) {
      std::cout << "impl PROVIDER" << std::endl;
    } else if constexpr (type == SourceType::CALCULATOR) {
      std::cout << "impl CALCULATOR" << std::endl;
    }
  }
};

template <typename Derived>
void handle(std::shared_ptr<ITest<Derived>>& cc) {
  cc->template Debug<SourceType::PROVIDER>(1);
}

int main() {
  Test c;
  c.Debug<SourceType::PROVIDER>(1);
  c.Debug<SourceType::CALCULATOR>(1);

  std::shared_ptr<ITest<Test>> cc = std::make_shared<Test>();
  cc->Debug<SourceType::PROVIDER>(1);

  handle(cc);
  return 0;
}
