#ifndef S11N_INCLUDE_ME_S11N_PAYLOAD_CODER_PAYLOAD_CODER_H
#define S11N_INCLUDE_ME_S11N_PAYLOAD_CODER_PAYLOAD_CODER_H
#include "../raw_coder/raw_coder.h"
#include "../utils/common.h"
namespace me {
namespace s11n {
template <typename T, typename Enable = void> struct PayloadCoder {
  static uint8_t *Write(const T &value, uint8_t *ptr) {
    return WriteRaw(value, ptr);
  }
  static const uint8_t *Read(T &out, const uint8_t *ptr) {
    return ReadRaw(out, ptr);
  }
  static std::size_t Size(const T &value) { return SizeRaw(value); }
};

template <typename T>
inline static uint8_t *WritePayload(const T &value, uint8_t *ptr) {
  return PayloadCoder<T>::Write(value, ptr);
}
template <typename T>
inline static const uint8_t *ReadPayload(T &out, const uint8_t *ptr) {
  return PayloadCoder<T>::Read(out, ptr);
}
template <typename T> inline static std::size_t SizePayload(const T &value) {
  return PayloadCoder<T>::Size(value);
}
template <typename T, std::size_t SIZE>
inline static uint8_t *WritePayload(const T (&value)[SIZE], uint8_t *ptr) {
  return PayloadCoder<T[SIZE]>::Write(value, ptr);
}
template <typename T, std::size_t SIZE>
inline static const uint8_t *ReadPayload(T (&out)[SIZE], const uint8_t *ptr) {
  return PayloadCoder<T[SIZE]>::Read(out, ptr);
}
template <typename T, std::size_t SIZE>
inline static constexpr std::size_t SizePayload(const T (&value)[SIZE]) {
  return PayloadCoder<T[SIZE]>::Size(value);
}

struct PayloadWrapper {
  static std::unordered_map<std::size_t, PayloadWrapper *> IdToCoderMap;
  static std::unordered_map<std::type_index, std::size_t> CppIdToIdMap;
  virtual uint8_t *Write(const void *value, uint8_t *ptr) = 0;
  virtual const uint8_t *Read(void *out, const uint8_t *ptr) = 0;
  virtual void *New() = 0;
};

template <typename T> struct PayloadWrapperImpl : public PayloadWrapper {
  uint8_t *Write(const void *value, uint8_t *ptr) override {
    return PayloadCoder<T>::Write(*reinterpret_cast<const T *>(value), ptr);
  }
  const uint8_t *Read(void *out, const uint8_t *ptr) override {
    return PayloadCoder<T>::Read(*reinterpret_cast<T *>(out), ptr);
  }
  void *New() override {
    // this means a serializable must has a default constructor
    return new T();
  }
};

// shared_ptrs are not supported
template <typename T> struct PayloadCoder<std::shared_ptr<T>> {
  static void CompileError() {
    static_assert(
        DeferredFalse<T>::value,
        "shared_ptr is not supported, serializable should owns something, not "
        "shares something.");
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

// non-polymorphic type do not supported
template <typename T, typename... TS>
struct PayloadCoder<
    std::unique_ptr<T, TS...>,
    typename std::enable_if<!std::is_polymorphic<T>::value>::type> {
  static void CompileError() {
    static_assert(
        DeferredFalse<T>::value,
        "due to ownership principle, ptr of a non polymorphic type makes "
        "little sense, please issue us if you has a reasonable case");
  }
  static uint8_t *Write(const std::unique_ptr<T, TS...> &unique_ptr,
                        uint8_t *ptr) {
    CompileError();
    return nullptr;
  }
  static const uint8_t *Read(std::unique_ptr<T, TS...> &unique_ptr,
                             const uint8_t *ptr) {
    CompileError();
    return nullptr;
  }
  static std::size_t Size(const std::unique_ptr<T, TS...> &unique_ptr) {
    CompileError();
    return 0;
  }
};
// unique_ptr Type for polymorphic type
template <typename T, typename... TS>
struct Coder<std::unique_ptr<T, TS...>,
             typename std::enable_if_t<std::is_polymorphic<T>::value>> {
  static uint8_t *Write(const std::unique_ptr<T, TS...> &unique_ptr,
                        uint8_t *ptr) {
    auto id = PayloadWrapper::CppIdToIdMap[typeid(*unique_ptr)];
    ptr = WriteRaw(id, ptr);
    ptr = PayloadWrapper::IdToCoderMap[id]->Write(unique_ptr.get(), ptr);
    return ptr;
  }
  static const uint8_t *Read(std::unique_ptr<T, TS...> &unique_ptr,
                             const uint8_t *ptr) {
    std::size_t id;
    ptr = ReadRaw(id, ptr);
    void *out = PayloadWrapper::IdToCoderMap[id]->New();
    ptr = PayloadWrapper::IdToCoderMap[id]->Read(out, ptr);
    unique_ptr.reset(reinterpret_cast<T *>(out));
    return ptr;
  }
  static std::size_t Size(const std::unique_ptr<T, TS...> &unique_ptr) {
    std::size_t id = PayloadWrapper::CppIdToIdMap[typeid(id)];
    return SizeRaw(id) + SizePayload(*unique_ptr);
  }
};
// encoder for arrays which element is varint
template <typename T, std::size_t SIZE>
struct Coder<T[SIZE], typename std::enable_if_t<GraininessWrapper<T>::type ==
                                                Graininess::VARINT>> {
  static uint8_t *Write(const T (&value)[SIZE], uint8_t *ptr) {
    // write size
    ptr = WriteRaw(SIZE, ptr);

    for (std::size_t i = 0; i < SIZE; i++) {
      ptr = WritePayload(value[i], ptr);
    }
    return ptr;
  }
  static const uint8_t *Read(T (&out)[SIZE], const uint8_t *ptr) {
    std::size_t size;
    ptr = ReadRaw(size, ptr);
    for (std::size_t i = 0; i < SIZE; i++) {
      ptr = ReadPayload(out[i], ptr);
    }
    return ptr;
  }
  static std::size_t Size(const T (&value)[SIZE]) {
    std::size_t total_size = 0;
    for (std::size_t i = 0; i < SIZE; i++) {
      total_size += SizePayload(value[i]);
    }
    total_size += SizeRaw(total_size);
    return total_size;
  }
};

// encoder for arrays which element is fixed
template <typename T, std::size_t SIZE>
    struct Coder < T[SIZE],
    typename std::enable_if_t<GraininessWrapper<T>::type<Graininess::VARINT>> {
  static uint8_t *Write(const T (&value)[SIZE], uint8_t *ptr) {
    // write size
    ptr = WriteRaw(SIZE, ptr);
    return EndianHelper<sizeof(T), ME_LITTLE_ENDIAN>::SavePacked(
        (const uint8_t *)&value[0], ptr, SIZE);
  }
  static const uint8_t *Read(T (&out)[SIZE], const uint8_t *ptr) {
    std::size_t size;
    ptr = Coder<decltype(SIZE)>::Read(size, ptr);
    return EndianHelper<sizeof(T), ME_LITTLE_ENDIAN>::LoadPacked((uint8_t *)out,
                                                                 ptr, SIZE);
  }
  static constexpr std::size_t Size(const T (&value)[SIZE]) {
    return Coder<decltype(SIZE)>::template ConstexprSize<SIZE>() +
           (SIZE * PayloadCoder<T>::Size(value[0]));
  }
};
// encoder for vectors which element is varint
template <typename T, typename... TS>
struct Coder<std::vector<T, TS...>,
             typename std::enable_if_t<GraininessWrapper<T>::type ==
                                       Graininess::VARINT>> {
  static uint8_t *Write(const std::vector<T> &v, uint8_t *ptr) {
    // write size
    ptr = Coder<decltype(v.size())>::Write(v.size(), ptr);
    for (auto i : v) {
      ptr = PayloadCoder<T>::Write(i, ptr);
    }
    return ptr;
  }
  static const uint8_t *Read(std::vector<T> &out, const uint8_t *ptr) {
    std::size_t size;
    ptr = Coder<decltype(size)>::Read(size, ptr);
    out.resize(size);
    for (auto &i : out) {
      ptr = PayloadCoder<T>::Read(i, ptr);
    }
    return ptr;
  }
  static std::size_t Size(const std::vector<T> &v) {
    auto vector_size = v.size();
    std::size_t total_size = Coder<decltype(vector_size)>::Size(vector_size);
    for (auto i : v) {
      total_size += PayloadCoder<T>::Size(i);
    }
    return total_size;
  }
};
// encoder for vectors which element is fixed
template <typename T, typename... TS>
    struct Coder < std::vector<T, TS...>,
    typename std::enable_if_t<GraininessWrapper<T>::type<Graininess::VARINT>> {
  static uint8_t *Write(const std::vector<T> &v, uint8_t *ptr) {
    ptr = Coder<decltype(v.size())>::Write(v.size(), ptr);
    return EndianHelper<sizeof(T), ME_LITTLE_ENDIAN>::SavePacked(
        (const uint8_t *)&v[0], ptr, v.size());
  }
  static const uint8_t *Read(std::vector<T> &out, const uint8_t *ptr) {
    std::size_t size;
    ptr = Coder<decltype(size)>::Read(size, ptr);
    out.resize(size);
    return EndianHelper<sizeof(T), ME_LITTLE_ENDIAN>::LoadPacked(
        (uint8_t *)out.data(), ptr, out.size());
  }
  static std::size_t Size(const std::vector<T> &v) {
    auto vector_size = v.size();
    return Coder<decltype(vector_size)>::Size(vector_size) +
           (vector_size * PayloadCoder<T>::Size(v));
  }
};
// coder for std::string
template <> struct Coder<std::string> {
  static uint8_t *Write(const std::string &s, uint8_t *ptr) {
    ptr = Coder<decltype(s.size())>::Write(s.size(), ptr);
    std::copy(s.begin(), s.begin() + s.size(), ptr);
    return ptr + s.size();
  }
  static const uint8_t *Read(std::string &out, const uint8_t *ptr) {
    std::size_t size;
    ptr = Coder<decltype(size)>::Read(size, ptr);
    out.resize(size);
    std::copy(ptr, ptr + size, &out[0]);
    return ptr + size;
  }
  static std::size_t Size(const std::string &s) {
    auto string_size = s.size();
    return Coder<decltype(string_size)>::Size(string_size) + s.size();
  }
};

// coder for std::pair
template <typename T1, typename T2> struct Coder<std::pair<T1, T2>> {
  static uint8_t *Write(const std::pair<T1, T2> &value, uint8_t *ptr) {
    ptr = Coder<T1>::Write(value.first, ptr);
    ptr = Coder<T2>::Write(value.second, ptr);
    return ptr;
  }
  static const uint8_t *Read(std::pair<T1, T2> &out, const uint8_t *ptr) {
    ptr = Coder<T1>::Read(out.first, ptr);
    ptr = Coder<T2>::Read(out.second, ptr);
    return ptr;
  }
  static std::size_t Size(const std::pair<T1, T2> &value) {
    return Coder<T1>::Size(value.first) + Coder<T2>::Size(value.second);
  }
};

// coder for maps
template <template <typename, typename, typename...> class MAP, typename KEY,
          typename VALUE, typename... TS>
struct Coder<
    MAP<KEY, VALUE, TS...>,
    typename std::enable_if_t<
        is_specialization<MAP<KEY, VALUE, TS...>, std::unordered_map>::value ||
        is_specialization<MAP<KEY, VALUE, TS...>, std::map>::value>> {
  static uint8_t *Write(const MAP<KEY, VALUE, TS...> &value, uint8_t *ptr) {
    ptr = WriteRaw(value.size(), ptr);
    for (const std::pair<KEY, VALUE> &pair : value) {
      ptr = Coder<std::pair<KEY, VALUE>>::Write(pair, ptr);
    }
    return ptr;
  }
  static const uint8_t *Read(MAP<KEY, VALUE, TS...> &out, const uint8_t *ptr) {
    std::size_t size;
    ptr = ReadRaw(size, ptr);
    std::pair<KEY, VALUE> pair;
    for (std::size_t i = 0; i < size; i++) {
      ptr = Coder<std::pair<KEY, VALUE>>::Read(pair, ptr);
      out.emplace(pair);
    }
    return ptr;
  }
  static std::size_t Size(const MAP<KEY, VALUE, TS...> &value) {
    std::size_t size = 0;
    size += SizeRaw(value.size());
    for (const std::pair<KEY, VALUE> &pair : value) {
      size += Coder<std::pair<KEY, VALUE>>::Size(pair);
    }
    return size;
  }
};
} // namespace s11n
} // namespace me
#endif // S11N_INCLUDE_ME_S11N_PAYLOAD_CODER_PAYLOAD_CODER_H
