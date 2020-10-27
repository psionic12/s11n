#include "demo_classes/rtti_classes.s11n.h"
#include <gtest/gtest.h>
#include <me/s11n/rtti_type_coder.h>
#include <me/s11n/utils/simple_buffer.h>
namespace {
class RttiTest : public testing::Test {};
me::s11n::SimpleBuffer buffer;
TEST_F(RttiTest, rtti_test) {
  std::unique_ptr<Base> s(new Base());
  auto size = me::s11n::Capacity(s);
  ASSERT_EQ(size, 9);
  buffer.ReSize(size);
  ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + size);
  std::unique_ptr<Base> t(nullptr);
  ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + size);
  ASSERT_EQ(t->Test(), s->Test());
  ASSERT_EQ(t->Test(), "Base");

  s.reset(new DerivedOne());
  size = me::s11n::Capacity(s);
  ASSERT_EQ(size, 15);
  buffer.ReSize(size);
  ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + size);
  ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + size);
  ASSERT_EQ(t->Test(), s->Test());
  ASSERT_EQ(t->Test(), "DerivedOne");

  s.reset(new DerivedTwo());
  size = me::s11n::Capacity(s);
  ASSERT_EQ(size, 15);
  buffer.ReSize(size);
  ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + size);
  ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + size);
  ASSERT_EQ(t->Test(), s->Test());
  ASSERT_EQ(t->Test(), "DerivedTwo");

  s.reset(nullptr);
  size = me::s11n::Capacity(s);
  ASSERT_EQ(size, 1);
  buffer.ReSize(size);
  ASSERT_EQ(me::s11n::Encode(s, buffer.Data()), buffer.Data() + size);
  ASSERT_EQ(me::s11n::Decode(t, buffer.Data()), buffer.Data() + size);
  ASSERT_EQ(t.get(), nullptr);
}
} // namespace