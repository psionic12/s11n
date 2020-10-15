#include <gtest/gtest.h>
#include <limits>
#include <me/s11n/type_coder.h>
#include <me/s11n/utils/simple_buffer.h>
namespace {
class BuiltInTypeTest : public testing::Test {};
me::s11n::SimpleBuffer buffer;
template <typename T>
std::tuple<T, std::size_t, uint8_t *, const uint8_t *>
RawTest(T t, std::size_t size) {
  T out;
  auto cap = me::s11n::Capacity(t);
  buffer.ReCapacity(cap);
  auto ptr1 = me::s11n::Encode(t, buffer.Data());
  auto ptr2 = me::s11n::Decode(out, buffer.Data());
  return {out, cap, ptr1, ptr2};
}
#define TestRaw(i, n)                                                          \
  {                                                                            \
    auto tuple = RawTest(i, n);                                                \
    ASSERT_EQ(i, std::get<0>(tuple));                                          \
    ASSERT_EQ(n, std::get<1>(tuple));                                          \
    ASSERT_EQ(buffer.Data() + n, std::get<2>(tuple));                          \
    ASSERT_EQ(buffer.Data() + n, std::get<3>(tuple));                          \
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

  // size not match test
  unsigned short int short_i = 128;
  buffer.ReCapacity(me::s11n::Capacity(short_i));
  me::s11n::Encode(short_i, buffer.Data());
  unsigned long int long_i;
  me::s11n::Decode(long_i, buffer.Data());
  ASSERT_EQ(short_i, long_i);

  long_i = 0xFFFFFFFF;
  buffer.ReCapacity(me::s11n::Capacity(long_i));
  me::s11n::Encode(long_i, buffer.Data());
  me::s11n::Decode(short_i, buffer.Data());
  ASSERT_EQ(short_i, 0xFFFF);
}

TEST_F(BuiltInTypeTest, ill_form_test) {
  buffer.ReCapacity(5);
  memset(buffer.Data(), 0, buffer.Capacity());
  buffer.Data()[0] = 0b11111111;
  buffer.Data()[1] = 0b10000000;
  buffer.Data()[2] = 0b10000000;
  uint64_t out;
  me::s11n::Decode(out, buffer.Data());
  ASSERT_EQ(out, 127);

  buffer.Data()[3] = 0b10000001;
  me::s11n::Decode(out, buffer.Data());
  ASSERT_EQ(out, 0b1000000000000001111111);
}

TEST_F(BuiltInTypeTest, enum_test) {
  enum class Foo {
    One,
    Two,
    Three,
  };
  Foo foo = Foo::Three;
  Foo out;
  buffer.ReCapacity(me::s11n::Capacity(foo));
  me::s11n::Encode(foo, buffer.Data());
  me::s11n::Decode(out, buffer.Data());
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

TEST_F(BuiltInTypeTest, array_test) {
  buffer.ReCapacity(32);
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    unsigned int s[] = {0, 1, 127, 128};
    ASSERT_EQ(me::s11n::Capacity(s), 6);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 6);
    unsigned int t[4] = {0, 0, 0, 0};
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 6);
    for (int index = 0; index < 4; index++) {
      ASSERT_EQ(s[index], t[index]);
    }
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    int s[] = {0, 1, 127, 128};
    ASSERT_EQ(me::s11n::Capacity(s), 7);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 7);
    int t[4] = {0, 0, 0, 0};
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 7);
    for (int index = 0; index < 4; index++) {
      ASSERT_EQ(s[index], t[index]);
    }
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    float s[] = {0.1, 1.0, 10.0};
    ASSERT_EQ(me::s11n::Capacity(s), 13);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 13);
    float t[] = {0, 0, 0};
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 13);
    for (int index = 0; index < 3; index++) {
      ASSERT_EQ(s[index], t[index]) << "differ at index " << index;
    }
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    unsigned int s[] = {0, 1, 127, 128};
    ASSERT_EQ(me::s11n::Capacity(s), 6);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 6);
    unsigned int t[4] = {0, 0, 0, 0};
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 6);
    for (int index = 0; index < 4; index++) {
      ASSERT_EQ(s[index], t[index]);
    }
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    int s[] = {-1, -11, -111, -1111, -11111};
    buffer.ReCapacity(10);
    ASSERT_EQ(me::s11n::Capacity(s), 10);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 10);
    int t[5] = {0, 0, 0, 0, 0};
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 10);
    for (int index = 0; index < 5; index++) {
      ASSERT_EQ(s[index], t[index]);
    }
  }
}

TEST_F(BuiltInTypeTest, vector_test) {
  buffer.ReCapacity(32);
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::vector<unsigned int> s = {0, 1, 127, 128};
    ASSERT_EQ(me::s11n::Capacity(s), 6);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 6);
    std::vector<unsigned int> t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 6);
    ASSERT_EQ(t, s);
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::vector<int> s = {0, 1, 127, 128};
    ASSERT_EQ(me::s11n::Capacity(s), 7);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 7);
    std::vector<int> t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 7);
    ASSERT_EQ(t, s);
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::vector<float> s = {0.01, 0.1, 1.1, 10.1};
    ASSERT_EQ(me::s11n::Capacity(s), 17);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 17);
    std::vector<float> t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 17);
    ASSERT_EQ(t, s);
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::vector<int> s = {};
    ASSERT_EQ(me::s11n::Capacity(s), 1);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 1);
    std::vector<int> t = {1, 2, 3, 4};
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 1);
    ASSERT_EQ(t, s);
  }
}

TEST_F(BuiltInTypeTest, string_test) {
  buffer.ReCapacity(32);
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::string s = "Hello, world!";
    ASSERT_EQ(me::s11n::Capacity(s), 14);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 14);
    std::string t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 14);
    ASSERT_EQ(t, s);
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::string s = u8"你好,世界!";
    ASSERT_EQ(me::s11n::Capacity(s), 15);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 15);
    std::string t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 15);
    ASSERT_EQ(t, s);
  }

  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::string s;
    ASSERT_EQ(me::s11n::Capacity(s), 1);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 1);
    std::string t = "not empty";
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 1);
    ASSERT_EQ(t, s);
  }
}

TEST_F(BuiltInTypeTest, pair_test) {
  buffer.ReCapacity(32);
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::pair<int, unsigned> s = {127, 128};
    ASSERT_EQ(me::s11n::Capacity(s), 5);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 5);
    std::pair<int, unsigned> t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 5);
    ASSERT_EQ(t, s);
  }
}

TEST_F(BuiltInTypeTest, map_test) {
  buffer.ReCapacity(32);
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::map<int, unsigned> s = {{127, 128}, {0, 1}};
    ASSERT_EQ(me::s11n::Capacity(s), 9);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 9);
    std::map<int, unsigned> t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 9);
    ASSERT_EQ(t, s);
  }
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::unordered_map<int, unsigned> s = {{127, 128}, {0, 1}};
    ASSERT_EQ(me::s11n::Capacity(s), 9);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 9);
    std::unordered_map<int, unsigned> t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 9);
    ASSERT_EQ(t, s);
  }
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::unordered_map<std::string, float> s = {{"one.one", 1.1},
                                                {"one.two", 1.2}};
    ASSERT_EQ(me::s11n::Capacity(s), 27);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 27);
    std::unordered_map<std::string, float> t;
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 27);
    ASSERT_EQ(t, s);
  }
  {
    me::s11n::SizeCache<>::WriteGuard guard;
    std::map<std::string, std::string> s = {};
    ASSERT_EQ(me::s11n::Capacity(s), 1);
    ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + 1);
    std::map<std::string, std::string> t = {{"a", "b"}};
    ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + 1);
    ASSERT_EQ(t, s);
  }
}

} // namespace