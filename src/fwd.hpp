#pragma once

#include <cstddef>
#include <type_traits>
#include <string>

namespace Machine
{
    using index_t = std::size_t;
    using integer_t = std::make_signed_t<index_t>;
    using character_t = unsigned short;

    static constexpr index_t negative_1 = static_cast<index_t>(-1);
    static constexpr index_t negative_2 = static_cast<index_t>(-2);

    [[noreturn]] void abort(const std::string &);


    static_assert(std::is_unsigned_v<character_t>);
}

