#include "demo_classes/rtti_classes.s11n.h"
#include <gtest/gtest.h>
#include <limits>
#include <me/s11n/field_coder.h>
#include <me/s11n/utils/simple_buffer.h>
namespace {
class FieldTest : public testing::Test {};
TEST_F(FieldTest, skip_test) {
  std::size_t size = 0;
  unsigned int f1 = 0;
  signed int f2 = 1;
  signed int f3 = 127;
  float f4 = 1.0;
  int f5[5] = {-1, -11, -111, -1111, -11111};
  std::vector<unsigned int> f6 = {1, 11, 111, 1111, 11111, 111111};
  std::vector<double> f7 = {0.001, 0.01, 0.1, 1.0, 10.0};
  std::string f8 = u8"大地の鼓動、大地の恵み\n"
                   "土に根を下ろし、世界の調和を知る\n"
                   "　\n"
                   "生命の息吹、たましいの風\n"
                   "吹き抜ける風の匂いが美を創り出す\n"
                   "　\n"
                   "我は大地の一部\n"
                   "我は空の一部\n"
                   "世界は己と共にあり、世界は己の中にある\n"
                   "我から意志は発ち、意志は我に還る\n"
                   "　\n"
                   "天の怒り、天の恵み\n"
                   "大いなる意志が世界の均衡を保つ\n"
                   "　\n"
                   "我は大地の一部\n"
                   "我は空の一部\n"
                   "世界は己と共にあり、世界は己の中にある\n"
                   "我から意志は発ち、意志は我に還る"; // the song I'm listening
  std::pair<std::string, double> f9 = {"key", 1.0};
  std::map<std::string, double> f10 = {{"one point two", 1.2},
                                       {"two point three", 2.3},
                                       {"three point four", 3.4}};
  std::unique_ptr<Base> f11(new DerivedTwo());

  me::s11n::SizeCache<>::WriteGuard guard;
  size += me::s11n::FieldCapacity<0>(f1);
  size += me::s11n::FieldCapacity<1>(f2);
  size += me::s11n::FieldCapacity<127>(f3);
  size += me::s11n::FieldCapacity<128>(f4);
  size += me::s11n::FieldCapacity<0x3FFF>(f5);
  size += me::s11n::FieldCapacity<0x4000>(f6);
  size += me::s11n::FieldCapacity<0x4001>(f7);
  size += me::s11n::FieldCapacity<0x4002>(f8);
  size += me::s11n::FieldCapacity<0x4003>(f9);
  size += me::s11n::FieldCapacity<0x4004>(f10);
  size += me::s11n::FieldCapacity<0x4005>(f11);

  me::s11n::SimpleBuffer buffer;
  buffer.ReCapacity(size);
  auto ptr = buffer.Data();
  ptr = me::s11n::EncodeField(0, f1, ptr);
  ptr = me::s11n::EncodeField(1, f2, ptr);
  ptr = me::s11n::EncodeField(127, f3, ptr);
  ptr = me::s11n::EncodeField(128, f4, ptr);
  ptr = me::s11n::EncodeField(0x3FFF, f5, ptr);
  ptr = me::s11n::EncodeField(0x4000, f6, ptr);
  ptr = me::s11n::EncodeField(0x4001, f7, ptr);
  ptr = me::s11n::EncodeField(0x4002, f8, ptr);
  ptr = me::s11n::EncodeField(0x4003, f9, ptr);
  ptr = me::s11n::EncodeField(0x4004, f10, ptr);
  ptr = me::s11n::EncodeField(0x4005, f11, ptr);

  const uint8_t *skip_ptr = buffer.Data();
  for (int i = 0; i < 11; i++) {
    skip_ptr = me::s11n::SkipVarint(skip_ptr);
    uint8_t tag;
    skip_ptr = me::s11n::Decode(tag, skip_ptr);
    skip_ptr = me::s11n::SkipUnknownField(tag, skip_ptr);
  }
  ASSERT_EQ(skip_ptr, ptr);
}
} // namespace