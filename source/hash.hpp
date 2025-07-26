#pragma once

#include <ranges>
#include <type_traits>

template<std::ranges::range range_type>
requires(std::integral<std::remove_cvref_t<decltype(*std::begin(std::declval<range_type>()))>>)
class std::hash<range_type>
{
private:
    template<std::integral type>
    static constexpr std::size_t compute_n()
    {
        using unsigned_type = std::make_unsigned_t<type>;
        std::size_t ret = std::numeric_limits<unsigned_type>::max() /
            std::numbers::phi_v<long double>;
        if (ret % 2 == 0)
            ++ret;
        return ret;
    }

public:
    std::size_t operator()(const range_type &range) const
    {
        static constexpr std::size_t n = compute_n<std::size_t>();

        std::size_t ret = std::size(range);

        for (auto i : range)
            ret ^= i + n + (ret << 6) + (ret >> 2);

        return ret;
    }
};

