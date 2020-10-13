#include <gtest/gtest.h>
#include <me/s11n/utils/endianness_helper.h>
#include <me/s11n/utils/log2_floor_helper.h>
#include <me/s11n/utils/size_cache.h>
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
  uint64_t reverse = 0xEFCDAB8967452301;
  uint8_t buffer[8];

  uint64_t out;
  me::s11n::EndianHelper<sizeof(origin), true>::Save(
      reinterpret_cast<const uint8_t *>(&origin), buffer);
  ASSERT_EQ(memcmp(buffer, reinterpret_cast<const uint8_t *>(&origin),
                   sizeof(origin)),
            0);
  me::s11n::EndianHelper<sizeof(origin), true>::Load(
      reinterpret_cast<uint8_t *>(&out), buffer);
  ASSERT_EQ(origin, out);

  me::s11n::EndianHelper<sizeof(origin), false>::Save(
      reinterpret_cast<const uint8_t *>(&origin), buffer);
  ASSERT_EQ(memcmp(buffer, reinterpret_cast<const uint8_t *>(&reverse),
                   sizeof(reverse)),
            0);
  me::s11n::EndianHelper<sizeof(origin), false>::Load(
      reinterpret_cast<uint8_t *>(&out), buffer);
  ASSERT_EQ(origin, out);
}
TEST_F(UtilsTest, endian_pack_test) {
  uint64_t origin[2] = {0x0123456789ABCDEF, 0xEFCDAB8967452301};
  uint64_t reverse[2] = {0xEFCDAB8967452301, 0x0123456789ABCDEF};
  uint8_t buffer[16];
  uint64_t out[2];

  auto ptr1 = me::s11n::EndianHelper<sizeof(uint64_t), true>::SavePacked(
      reinterpret_cast<const uint8_t *>(&origin), buffer, 2);
  ASSERT_EQ(memcmp(buffer, reinterpret_cast<const uint8_t *>(&origin),
                   sizeof(uint8_t) * 16),
            0);
  auto ptr2 = me::s11n::EndianHelper<sizeof(uint64_t), true>::LoadPacked(
      reinterpret_cast<uint8_t *>(&out), buffer, 2);
  ASSERT_EQ(memcmp(out, origin, sizeof(uint8_t) * 16), 0);
  ASSERT_EQ(ptr1, ptr2);
  ASSERT_EQ(ptr1, buffer + 16);

  ptr1 = me::s11n::EndianHelper<sizeof(uint64_t), false>::SavePacked(
      reinterpret_cast<const uint8_t *>(&origin), buffer, 2);
  ASSERT_EQ(memcmp(buffer, reinterpret_cast<const uint8_t *>(&reverse),
                   sizeof(uint8_t) * 16),
            0);
  ptr2 = me::s11n::EndianHelper<sizeof(uint64_t), false>::LoadPacked(
      reinterpret_cast<uint8_t *>(&out), buffer, 2);
  ASSERT_EQ(memcmp(out, origin, sizeof(uint8_t) * 16), 0);
  ASSERT_EQ(ptr1, ptr2);
  ASSERT_EQ(ptr1, buffer + 16);
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

TEST_F(UtilsTest, size_cache_test) {
  int i[] = {128, 0};
  me::s11n::SizeCache<>::Set(i, 3);
  me::s11n::SizeCache<>::Set(i[0], 2);
  me::s11n::SizeCache<>::Set(i[1], 1);

  ASSERT_EQ(3, me::s11n::SizeCache<>::Get(i));
  ASSERT_EQ(2, me::s11n::SizeCache<>::Get(i[0]));
  ASSERT_EQ(1, me::s11n::SizeCache<>::Get(i[1]));
}
} // namespace