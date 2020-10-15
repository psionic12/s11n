#ifndef S11N_TEST_DEMO_CLASSES_FOO_S11N_H
#define S11N_TEST_DEMO_CLASSES_FOO_S11N_H
#include "foo.h"
#include "me/s11n/field_coder.h"
#include <cstdint>
namespace me {
namespace s11n {
template <> struct TypeCoder<Foo> {
  static uint8_t *Write(const Foo &value, uint8_t *ptr) {
    ptr = Encode(SizeCache<>::Get(value), ptr);
    ptr = EncodeField(1, value.f1, ptr);
    ptr = EncodeField(2, value.f2, ptr);
    ptr = EncodeField(3, value.f3, ptr);
    ptr = EncodeField(4, value.f4, ptr);
    ptr = EncodeField(5, value.f5, ptr);
    ptr = EncodeField(6, value.f6, ptr);
    ptr = EncodeField(7, value.f7, ptr);
    ptr = EncodeField(8, value.f8, ptr);
    ptr = EncodeField(9, value.f9, ptr);
    ptr = EncodeField(10, value.f10, ptr);
    ptr = EncodeField(11, value.f11, ptr);
    ptr = EncodeField(12, value.f12, ptr);
    ptr = EncodeField(13, value.f13, ptr);
    ptr = EncodeField(14, value.f14, ptr);
    ptr = EncodeField(15, value.f15, ptr);
    return ptr;
  }
  static const uint8_t *Read(Foo &out, const uint8_t *ptr) {
    std::size_t total_size;
    const uint8_t * begin = ptr;
    ptr = Decode(total_size, ptr);
    const uint8_t *ptr_end = begin + total_size;
    while (ptr < ptr_end) {
      uint32_t index;
      ptr = Decode(index, ptr);
      uint8_t tag;
      ptr = Decode(tag, ptr);
      switch (index) {
        // TODO unlikely tag check
      case 0:
        break;
      case 1:
        ptr = Decode(out.f1, ptr);
        break;
      case 2:
        ptr = Decode(out.f2, ptr);
        break;
      case 3:
        ptr = Decode(out.f3, ptr);
        break;
      case 4:
        ptr = Decode(out.f4, ptr);
        break;
      case 5:
        ptr = Decode(out.f5, ptr);
        break;
      case 6:
        ptr = Decode(out.f6, ptr);
        break;
      case 7:
        ptr = Decode(out.f7, ptr);
        break;
      case 8:
        ptr = Decode(out.f8, ptr);
        break;
      case 9:
        ptr = Decode(out.f9, ptr);
        break;
      case 10:
        ptr = Decode(out.f10, ptr);
        break;
      case 11:
        ptr = Decode(out.f11, ptr);
        break;
      case 12:
        ptr = Decode(out.f12, ptr);
        break;
      case 13:
        ptr = Decode(out.f13, ptr);
        break;
      case 14:
        ptr = Decode(out.f14, ptr);
        break;
        // skip test
        //      case 15:
        //        ptr = Decode(out.f15, ptr);
        //        break;
      default: {
        ptr = SkipUnknownField(tag, ptr);
      }
      }
    }
    return ptr;
  }
  static std::size_t PayloadSize(const Foo &value) {
    std::size_t payload_size = 0;
    payload_size += FieldCapacity<1>(value.f1);
    payload_size += FieldCapacity<2>(value.f2);
    payload_size += FieldCapacity<3>(value.f3);
    payload_size += FieldCapacity<4>(value.f4);
    payload_size += FieldCapacity<5>(value.f5);
    payload_size += FieldCapacity<6>(value.f6);
    payload_size += FieldCapacity<7>(value.f7);
    payload_size += FieldCapacity<8>(value.f8);
    payload_size += FieldCapacity<9>(value.f9);
    payload_size += FieldCapacity<10>(value.f10);
    payload_size += FieldCapacity<11>(value.f11);
    payload_size += FieldCapacity<12>(value.f12);
    payload_size += FieldCapacity<13>(value.f13);
    payload_size += FieldCapacity<14>(value.f14);
    payload_size += FieldCapacity<15>(value.f15);
    SizeCache<>::Set(value, payload_size);
    return payload_size;
  }
  static std::size_t Size(const Foo &value) {
    auto size = PayloadSize(value);
    size += Capacity(size);
    return size;
  }
};
} // namespace s11n
} // namespace me
#endif // S11N_TEST_DEMO_CLASSES_FOO_S11N_H
