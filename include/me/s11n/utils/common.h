#ifndef S11N_INCLUDE_ME_S11N_UTILS_COMMON_H
#define S11N_INCLUDE_ME_S11N_UTILS_COMMON_H
#include <type_traits>
namespace me {
namespace s11n {
// helper to avoid static_assert trigger directly
template <typename> struct DeferredFalse : std::false_type {};
} // namespace s11n
} // namespace me
#endif // S11N_INCLUDE_ME_S11N_UTILS_COMMON_H
