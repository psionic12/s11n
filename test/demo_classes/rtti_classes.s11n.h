#ifndef S11N_TEST_CUSTOM_RECORDS_S11N_H
#define S11N_TEST_CUSTOM_RECORDS_S11N_H
#include "rtti_classes.h"
#include <me/s11n/field_coder.h>
#include <me/s11n/rtti_type_coder.h>
namespace me {
namespace s11n {
template <> struct TypeCoder<Base> {
  static uint8_t *Write(const Base &value, uint8_t *ptr) {
    ptr = Encode(SizeCache<>::Get(value), ptr);
    ptr = EncodeField(0, value.msg, ptr);
    return ptr;
  }
  static const uint8_t *Read(Base &out, const uint8_t *ptr) {
    std::size_t total_size;
    ptr = Decode(total_size, ptr);
    const uint8_t *ptr_end = ptr + total_size;
    while (ptr < ptr_end) {
      uint32_t index;
      ptr = Decode(index, ptr);
      uint8_t tag;
      ptr = Decode(tag, ptr);
      switch (index) {
      case 0:
        // TODO unlikely tag check
        ptr = Decode(out.msg, ptr);
        break;
      default: {
        ptr = SkipUnknownField(tag, ptr);
      }
      }
    }
    return ptr;
  }
  static std::size_t PayloadSize(const Base &value) {
    std::size_t payload_size = FieldCapacity<1>(value.msg);
    SizeCache<>::Set(value, payload_size);
    return payload_size;
  }
  static std::size_t Size(const Base &value) {
    auto size = PayloadSize(value);
    size += Capacity(size);
    return size;
  }
};
template <> struct TypeCoder<DerivedOne> {
  static uint8_t *Write(const DerivedOne &value, uint8_t *ptr) {
    ptr = Encode(SizeCache<>::Get(value), ptr);
    ptr = EncodeField(0, value.msg, ptr);
    return ptr;
  }
  static const uint8_t *Read(DerivedOne &out, const uint8_t *ptr) {
    std::size_t total_size;
    ptr = Decode(total_size, ptr);
    const uint8_t *ptr_end = ptr + total_size;
    while (ptr < ptr_end) {
      uint32_t index;
      ptr = Decode(index, ptr);
      uint8_t tag;
      ptr = Decode(tag, ptr);
      switch (index) {
      case 0:
        // TODO unlikely tag check
        ptr = Decode(out.msg, ptr);
        break;
      default: {
        ptr = SkipUnknownField(tag, ptr);
      }
      }
    }
    return ptr;
  }
  static std::size_t PayloadSize(const DerivedOne &value) {
    std::size_t payload_size = FieldCapacity<1>(value.msg);
    SizeCache<>::Set(value, payload_size);
    return payload_size;
  }
  static std::size_t Size(const DerivedOne &value) {
    auto size = PayloadSize(value);
    size += Capacity(size);
    return size;
  }
};
template <> struct TypeCoder<DerivedTwo> {
  static uint8_t *Write(const DerivedTwo &value, uint8_t *ptr) {
    ptr = Encode(SizeCache<>::Get(value), ptr);
    ptr = EncodeField(0, value.msg, ptr);
    return ptr;
  }
  static const uint8_t *Read(DerivedTwo &out, const uint8_t *ptr) {
    std::size_t total_size;
    ptr = Decode(total_size, ptr);
    const uint8_t *ptr_end = ptr + total_size;
    while (ptr < ptr_end) {
      uint32_t index;
      ptr = Decode(index, ptr);
      uint8_t tag;
      ptr = Decode(tag, ptr);
      switch (index) {
      case 0:
        // TODO unlikely tag check
        ptr = Decode(out.msg, ptr);
        break;
      default: {
        ptr = SkipUnknownField(tag, ptr);
      }
      }
    }
    return ptr;
  }
  static std::size_t PayloadSize(const DerivedTwo &value) {
    std::size_t payload_size = FieldCapacity<1>(value.msg);
    SizeCache<>::Set(value, payload_size);
    return payload_size;
  }
  static std::size_t Size(const DerivedTwo &value) {
    auto size = PayloadSize(value);
    size += Capacity(size);
    return size;
  }
};
} // namespace s11n
} // namespace me
#endif // S11N_TEST_CUSTOM_RECORDS_S11N_H
