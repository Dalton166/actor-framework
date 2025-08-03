// This file is part of CAF, the C++ Actor Framework. See the file LICENSE in
// the main distribution directory for license terms and copyright or visit
// https://github.com/actor-framework/actor-framework/blob/main/LICENSE.

#include "caf/byte_span.hpp"

#include "caf/detail/rfc3629.hpp"

#include <algorithm>

namespace caf {

bool is_valid_utf8(const_byte_span bytes) noexcept {
  return detail::rfc3629::valid(bytes);
}

bool is_valid_ascii(const_byte_span buffer) noexcept {
  auto pred = [](std::byte bval) {
    return static_cast<unsigned char>(bval) <= 0x7F;
  };
  return std::all_of(buffer.begin(), buffer.end(), pred);
}

std::string_view to_string_view(const_byte_span bytes) noexcept {
  return std::string_view{reinterpret_cast<const char*>(bytes.data()),
                          bytes.size()};
}

const_byte_span to_const_byte_span(std::string_view str) noexcept {
  return {reinterpret_cast<const std::byte*>(str.data()), str.size()};
}

} // namespace caf
