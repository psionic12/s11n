#include <gtest/gtest.h>
#include <me/s11n/utils/endianness_helper.h>
#include <me/s11n/utils/log2_floor_helper.h>
namespace {
class UtilsTest : public testing::Test {};
TEST_F(UtilsTest, reverse_endian8_test) {
  uint8_t source = 0x01;
  uint8_t target = 0;
  me::s11n::ReverseEndian<sizeof(source)>(
      reinterpret_cast<const uint8_t *>(&source),
      reinterpret_cast<uint8_t *>(&target));
  ASSERT_EQ(target, 0x01);
}
TEST_F(UtilsTest, reverse_endian16_test) {
  uint16_t source = 0x0123;
  uint16_t target = 0;
  me::s11n::ReverseEndian<sizeof(source)>(
      reinterpret_cast<const uint8_t *>(&source),
      reinterpret_cast<uint8_t *>(&target));
  ASSERT_EQ(target, 0x2301);
}
TEST_F(UtilsTest, reverse_endian32_test) {
  uint32_t source = 0x01234567;
  uint32_t target = 0;
  me::s11n::ReverseEndian<sizeof(source)>(
      reinterpret_cast<const uint8_t *>(&source),
      reinterpret_cast<uint8_t *>(&target));
  ASSERT_EQ(target, 0x67452301);
}
TEST_F(UtilsTest, reverse_endian64_test) {
  uint64_t source = 0x0123456789ABCDEF;
  uint64_t target = 0;
  me::s11n::ReverseEndian<sizeof(source)>(
      reinterpret_cast<const uint8_t *>(&source),
      reinterpret_cast<uint8_t *>(&target));
  ASSERT_EQ(target, 0xEFCDAB8967452301);
}
TEST_F(UtilsTest, endian_helper_test) {
  uint64_t origin = 0x0123456789ABCDEF;
  uint8_t buffer[8];

  me::s11n::EndianHelper<sizeof(origin), true>::Save(
      reinterpret_cast<const uint8_t *>(&origin), buffer);
  ASSERT_EQ(memcmp(buffer, reinterpret_cast<const uint8_t *>(&origin),
                   sizeof(origin)),
            0);

  uint64_t reverse = 0xEFCDAB8967452301;
  me::s11n::EndianHelper<sizeof(reverse), false>::Save(
      reinterpret_cast<const uint8_t *>(&reverse), buffer);
  ASSERT_EQ(memcmp(buffer, reinterpret_cast<const uint8_t *>(&origin),
                   sizeof(reverse)),
            0);
}
TEST_F(UtilsTest, log2_floor_test) {
  // platform support log2Floor
  int i = 0;
  ASSERT_EQ(0, me::s11n::Log2FloorNonZero(i | 1));
  i = 1;
  ASSERT_EQ(0, me::s11n::Log2FloorNonZero(i));
  i = 128;
  ASSERT_EQ(7, me::s11n::Log2FloorNonZero(i));
  i = -1;
  ASSERT_EQ(sizeof(std::size_t) * 8 - 1, me::s11n::Log2FloorNonZero(i));
  unsigned long l = 0x0123456789ABCDEF;
  ASSERT_EQ(56, me::s11n::Log2FloorNonZero(l));

  // constexpr test
  ASSERT_EQ(0, me::s11n::Log2FloorNonZero<1>());
  ASSERT_EQ(7, me::s11n::Log2FloorNonZero<128>());
  ASSERT_EQ(56, me::s11n::Log2FloorNonZero<0x0123456789ABCDEF>());
}
} // namespace