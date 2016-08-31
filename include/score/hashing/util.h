#pragma once
#include <type_traits>
#include <map>

namespace score { namespace hashing {

template<
  typename T,
  typename TIgnore = typename std::enable_if<std::is_enum<T>::value>::type
>
size_t hashEnum(T enumVal) {
  using underlying = typename std::underlying_type<T>::type;
  return std::hash<underlying>()(
    static_cast<underlying>(enumVal)
  );
}

// stolen from boost::hash
size_t hashCombine(size_t seed, size_t hashedVal);

}} // score::hashing

