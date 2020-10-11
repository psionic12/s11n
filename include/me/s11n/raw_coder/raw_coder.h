#ifndef S11N_INCLUDE_ME_S11N_RAW_CODER_RAW_CODER_H
#define S11N_INCLUDE_ME_S11N_RAW_CODER_RAW_CODER_H
#include "../utils/common.h"
#include "../utils/endianness_helper.h"
#include "../utils/log2_floor_helper.h"
#include "../utils/port.h"
#include "builtin_types_helper.h"
#include <cstdint>
#include <memory>
namespace me {
namespace s11n {
// all unsupported types will fall into this Coder
template <typename T, typename Enable = void> struct RawCoder {
  static void CompileError() {
    static_assert(
        DeferredFalse<T>::value,
        "unsupported raw type, you probably should use PlayloadCoder instead");
  }
  static uint8_t *Write(const T &value, uint8_t *ptr) {
    CompileError();
    return nullptr;
  }
  static const uint8_t *Read(T &out, const uint8_t *ptr) {
    CompileError();
    return nullptr;
  }
  static std::size_t Size(const T &value) {
    CompileError();
    return 0;
  }
};

template <typename T>
inline static uint8_t *WriteRaw(const T &value, uint8_t *ptr) {
  return RawCoder<T>::Write(value, ptr);
}
template <typename T>
inline static const uint8_t *ReadRaw(T &out, const uint8_t *ptr) {
  return RawCoder<T>::Read(out, ptr);
}
template <typename T> inline static std::size_t SizeRaw(const T &value) {
  return RawCoder<T>::Size(value);
}
template <typename T, std::size_t SIZE>
inline static uint8_t *WriteRaw(const T (&value)[SIZE], uint8_t *ptr) {
  return RawCoder<T[SIZE]>::Write(value, ptr);
}
template <typename T, std::size_t SIZE>
inline static const uint8_t *ReadRaw(T (&out)[SIZE], const uint8_t *ptr) {
  return RawCoder<T[SIZE]>::Read(out, ptr);
}
template <typename T, std::size_t SIZE>
inline static constexpr std::size_t SizeRaw(const T (&value)[SIZE]) {
  return RawCoder<T[SIZE]>::Size(value);
}

// more specific unsupported types

// pointers are not supported
template <typename T> struct RawCoder<T *> {
  static void CompileError() {
    static_assert(
        DeferredFalse<T>::value,
        "pointer is not supported, serializable should owns something, not "
        "pointers something, use std::unique ptr instead.");
  }
  static uint8_t *Write(const T &value, uint8_t *ptr) {
    CompileError();
    return nullptr;
  }
  static const uint8_t *Read(T &out, const uint8_t *ptr) {
    CompileError();
    return nullptr;
  }
  static std::size_t Size(const T &value) {
    CompileError();
    return 0;
  }
};

// varint coder, used for unsigned arithmetic type
template <typename T>
struct RawCoder<T, typename std::enable_if<GraininessWrapper<T>::type ==
                                               Graininess::VARINT &&
                                           std::is_unsigned<T>::value>::type> {
  static uint8_t *Write(T value, uint8_t *ptr) {
    if (value < 0x80) {
      ptr[0] = static_cast<uint8_t>(value);
      return ptr + 1;
    }
    ptr[0] = static_cast<uint8_t>(value | 0x80);
    value >>= 7;
    if (value < 0x80) {
      ptr[1] = static_cast<uint8_t>(value);
      return ptr + 2;
    }
    ptr++;
    do {
      *ptr = static_cast<uint8_t>(value | 0x80);
      value >>= 7;
      ++ptr;
    } while (/*unlikely*/ (value >= 0x80));
    *ptr++ = static_cast<uint8_t>(value);
    return ptr;
  }
  static const uint8_t *Read(T &out, const uint8_t *ptr) {
    uint32_t res32 = ptr[0];
    if (!(res32 & 0x80)) {
      out = res32;
      return ptr + 1;
    }
    uint32_t byte1 = ptr[1];
    res32 += (byte1 - 1) << 7;
    if (!(byte1 & 0x80)) {
      out = res32;
      return ptr + 2;
    }
    // VarintParseSlow64 in protobuf code
    uint64_t res64 = res32;
    for (std::uint32_t i = 2; i < 10; i++) {
      uint64_t byte2 = static_cast<uint8_t>(ptr[i]);
      res64 += (byte2 - 1) << (7 * i);
      if (/*likely*/ (byte2 < 128)) {
        out = res64;
        return ptr + i + 1;
      }
    }
    out = 0;
    return nullptr;
  }
  static std::size_t Size(const T &value) {
    // same as VarintSizeXX in protobuf
    std::size_t log2value = Log2FloorNonZero(value | 0x1);
    // division to multiplication optimize
    return static_cast<std::size_t>((log2value * 9 + 73) / 64);
  }
  template <std::size_t SIZE> static constexpr std::size_t Size() {
    return static_cast<std::size_t>((Log2FloorNonZero<SIZE>() * 9 + 73) / 64);
  }
};

// zig-zag varint encoder, used for signed arithmetic type
template <typename T>
struct RawCoder<T, typename std::enable_if<GraininessWrapper<T>::type ==
                                               Graininess::VARINT &&
                                           std::is_signed<T>::value>::type> {
  typedef typename std::make_unsigned<T>::type UnsignedT;
  static inline constexpr UnsignedT ZigZagValue(T value) {
    return (static_cast<UnsignedT>(value) << 1) ^
           static_cast<UnsignedT>(value >> 31);
  }
  static uint8_t *Write(T value, uint8_t *ptr) {
    // convert to zig zag value
    UnsignedT zig_zag_value = ZigZagValue(value);
    // now call to unsigned version of WriteGraininessToArray
    return RawCoder<UnsignedT>::Write(zig_zag_value, ptr);
  }
  static const uint8_t *Read(T &out, const uint8_t *ptr) {
    UnsignedT zig_zag_value;
    auto temp = RawCoder<UnsignedT>::Read(zig_zag_value, ptr);
    // convert from zig zag value to normal value
    out = static_cast<T>((zig_zag_value >> 1) ^ (~(zig_zag_value & 1) + 1));
    return temp;
  }
  static std::size_t Size(const T &value) {
    UnsignedT zig_zag_value = ZigZagValue(value);
    return RawCoder<UnsignedT>::Size(zig_zag_value);
  }
  template <std::size_t SIZE> static constexpr std::size_t ConstexprSize() {
    return RawCoder<UnsignedT>::template ConstexprSize<ZigZagValue(SIZE)>();
  }
};

// enum type encoder
template <typename T>
struct RawCoder<T, typename std::enable_if<std::is_enum<T>::value>::type> {
  static uint8_t *Write(const T &value, uint8_t *ptr) {
    return RawCoder<unsigned>::Write(static_cast<unsigned>(value), ptr);
  }
  static const uint8_t *Read(T &out, const uint8_t *ptr) {
    unsigned index;
    ptr = RawCoder<unsigned>::Read(index, ptr);
    out = static_cast<T>(index);
    return ptr;
  }
  static constexpr std::size_t Size(T value) {
    return RawCoder<unsigned>::Size(static_cast<unsigned>(value));
  }
};

// fixed size encoder
template <typename T>
struct RawCoder<
    T, typename std::enable_if<(GraininessWrapper<T>::type < /* less than*/
                                Graininess::VARINT)>::type> {
  static uint8_t *Write(T value, uint8_t *ptr) {
    return EndianHelper<sizeof(T), ME_LITTLE_ENDIAN>::Save((uint8_t *)&value,
                                                           ptr);
  }
  static const uint8_t *Read(T &out, const uint8_t *ptr) {
    return EndianHelper<sizeof(T), ME_LITTLE_ENDIAN>::Load((uint8_t *)&out,
                                                           ptr);
  }
  static constexpr std::size_t Size(T value) { return sizeof(T); }
};
} // namespace s11n
} // namespace me
#endif // S11N_INCLUDE_ME_S11N_RAW_CODER_RAW_CODER_H
