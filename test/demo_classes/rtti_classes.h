#ifndef S11N_TEST_CUSTOM_RECORDS_H
#define S11N_TEST_CUSTOM_RECORDS_H
#include <string>
class Base {
public:
  std::string msg = "Base";
  virtual std::string Test() { return msg; }
};
class DerivedOne : public Base {
public:
  std::string msg = "DerivedOne";
  std::string Test() override { return msg; }
};
class DerivedTwo : public Base {
public:
  std::string msg = "DerivedTwo";
  std::string Test() override { return msg; }
};
#endif // S11N_TEST_CUSTOM_RECORDS_H
