#include <gtest/gtest.h>
#include <limits>
#include <me/s11n/raw_coder/raw_coder.h>
namespace {
class BuiltInTypeTest : public testing::Test {};
uint8_t buffer[8];
template <typename T> std::pair<T, std::size_t> RawTest(T t, std::size_t size) {
  T out;
  me::s11n::WriteRaw(t, buffer);
  me::s11n::ReadRaw(out, buffer);
  return {out, me::s11n::SizeRaw(t)};
}
#define TestRaw(i, n)                                                          \
  {                                                                            \
    auto pair = RawTest(i, n);                                                 \
    ASSERT_EQ(i, pair.first);                                                  \
    ASSERT_EQ(n, pair.second);                                                 \
  }
TEST_F(BuiltInTypeTest, integal_test) {
  unsigned short i = 0;
  TestRaw(i, 1);
  i = 1;
  TestRaw(i, 1);
  i = 0b01111111;
  TestRaw(i, 1);
  i = 0b11111111;
  TestRaw(i, 2);
  i = std::numeric_limits<decltype(i)>::max();
  TestRaw(i, 3);

  signed short si = 0;
  TestRaw(si, 1);
  si = 1;
  TestRaw(si, 1);
  si = 0b01111111;
  TestRaw(si, 2);
  si = 0b11111111;
  TestRaw(si, 2);
  si = std::numeric_limits<decltype(si)>::max();
  TestRaw(si, 3);

  me::s11n::strong_int32 strong_int32 = 0b11111111;
  TestRaw(strong_int32, 4);
  strong_int32 = -1;
  TestRaw(strong_int32, 4);
}
TEST_F(BuiltInTypeTest, enum_test) {
  enum class Foo {
    One,
    Two,
    Three,
  };
  Foo foo = Foo::Three;
  Foo out;
  me::s11n::WriteRaw(foo, buffer);
  me::s11n::ReadRaw(out, buffer);
  switch (out) {
  case Foo::One:
    ASSERT_FALSE(true);
    break;
  case Foo::Two:
    ASSERT_FALSE(true);
    break;
  case Foo::Three:
    ASSERT_TRUE(true);
    break;
  default:
    ASSERT_FALSE(true);
    break;
  }
}

} // namespace