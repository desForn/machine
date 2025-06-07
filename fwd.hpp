#pragma once
#include <cstddef>
#include <type_traits>

namespace Machine
{
    using index_t = std::size_t;
    using integer_t = std::make_unsigned_t<index_t>;
    using character_t = unsigned short;
    static constexpr index_t negative_1 = static_cast<index_t>(-1);
}

