#pragma once
#include "encoder.hpp"
#include "hash.hpp"

#include <vector>
#include <stdexcept>
#include <array>
#include <limits>

namespace Machine
{
    class unsigned_number_t;
    class signed_number_t;
    class b_ary_t {};
    class b_adic_t {};

    class unsigned_number_t final
    {
        friend unsigned_number_t operator*(const unsigned_number_t &, const unsigned_number_t &);
        friend std::array<unsigned_number_t, 2> divide(unsigned_number_t, index_t);
        friend std::array<unsigned_number_t, 2> divide(unsigned_number_t, unsigned_number_t);
        friend class signed_number_t;

    private:
        static constexpr index_t n_digits = std::numeric_limits<index_t>::digits;
        static constexpr index_t n_digits_half = n_digits / 2;
        static_assert(n_digits % 2 == 0);
        static constexpr index_t max = std::numeric_limits<index_t>::max();
        static constexpr std::array<index_t, 2> mask{max >> (n_digits / 2), max << (n_digits / 2)};
        static constexpr index_t max_half = mask.front();

        std::vector<index_t> digits_{};

    public:
        unsigned_number_t() = default;
        ~unsigned_number_t() = default;

        unsigned_number_t(const unsigned_number_t &) = default;
        unsigned_number_t &operator=(const unsigned_number_t &) = default;

        unsigned_number_t(unsigned_number_t &&) noexcept = default;
        unsigned_number_t &operator=(unsigned_number_t &&) noexcept = default;

        unsigned_number_t(index_t);
        unsigned_number_t &operator=(index_t);

        template<integer_t stride, bool is_const>
        unsigned_number_t(Apparatus::string_iterator_template_t<stride, is_const>,
            Apparatus::string_iterator_template_t<stride, is_const>, const encoder_t &);

        unsigned_number_t(std::string_view, const encoder_t &);

        void swap(unsigned_number_t &) noexcept;

    public:
        const std::vector<index_t> &digits() const;
        bool zero() const;

        string_t b_ary(character_t) const;
        string_t b_adic(character_t) const;

        std::string print_b_ary(character_t) const;
        std::string print_b_adic(character_t) const;
        std::string print(const encoder_t &) const;

    private:
        index_t load_half_word(index_t) const;
        void store_half_word(index_t, index_t);
        void normalise();

    public:
        bool operator==(const unsigned_number_t &) const = default;
        bool operator!=(const unsigned_number_t &) const = default;
        std::strong_ordering operator<=>(const unsigned_number_t &) const;

        unsigned_number_t &operator++();
        unsigned_number_t &operator--();
        unsigned_number_t operator++(int);
        unsigned_number_t operator--(int);

        unsigned_number_t &operator+=(const unsigned_number_t &);
        unsigned_number_t &operator-=(const unsigned_number_t &);
        unsigned_number_t &operator*=(const unsigned_number_t &);
        unsigned_number_t &operator/=(unsigned_number_t);
        unsigned_number_t &operator%=(unsigned_number_t);
        unsigned_number_t &operator/=(index_t);
        unsigned_number_t &operator%=(index_t);
        unsigned_number_t &operator<<=(integer_t);
        unsigned_number_t &operator>>=(integer_t);
    };

    unsigned_number_t operator+(unsigned_number_t, const unsigned_number_t &);
    unsigned_number_t operator-(unsigned_number_t, const unsigned_number_t &);
    unsigned_number_t operator*(const unsigned_number_t &, const unsigned_number_t &);
    unsigned_number_t operator/(unsigned_number_t, unsigned_number_t);
    unsigned_number_t operator%(unsigned_number_t, unsigned_number_t);
    unsigned_number_t operator/(unsigned_number_t, integer_t);
    unsigned_number_t operator%(unsigned_number_t, integer_t);
    unsigned_number_t operator<<(unsigned_number_t, integer_t);
    unsigned_number_t operator>>(unsigned_number_t, integer_t);

    std::array<unsigned_number_t, 2> divide(unsigned_number_t, index_t);
    std::array<unsigned_number_t, 2> divide(unsigned_number_t, unsigned_number_t);

    void swap(unsigned_number_t &, unsigned_number_t &) noexcept;

    class signed_number_t final
    {
        friend std::array<signed_number_t, 2> divide(signed_number_t, signed_number_t);
    private:
        static constexpr bool pos_ = false;
        static constexpr bool neg_ = true;

        bool sign_{pos_};
        unsigned_number_t magnitude_{};

    public:
        signed_number_t() = default;
        ~signed_number_t() = default;

        signed_number_t(const signed_number_t &) = default;
        signed_number_t &operator=(const signed_number_t &) = default;

        signed_number_t(signed_number_t &&) noexcept = default;
        signed_number_t &operator=(signed_number_t &&) noexcept = default;

        signed_number_t(integer_t);
        signed_number_t &operator=(integer_t);

        signed_number_t(unsigned_number_t);
        signed_number_t &operator=(unsigned_number_t);

        template<integer_t stride, bool is_const>
        signed_number_t(Apparatus::string_iterator_template_t<stride, is_const>,
            Apparatus::string_iterator_template_t<stride, is_const>, const encoder_t &);

        signed_number_t(std::string_view, const encoder_t &);

        void swap(signed_number_t &) noexcept;

    public:
        const unsigned_number_t &abs() const &;
        unsigned_number_t abs() &&;
        bool zero() const;
        bool pos() const;
        bool neg() const;

        string_t b_ary(character_t) const;
        string_t b_adic(character_t) const;

        std::string print_b_ary(character_t) const;
        std::string print_b_adic(character_t) const;
        std::string print(const encoder_t &) const;

    public:
        bool operator==(const signed_number_t &) const;
        bool operator!=(const signed_number_t &) const;
        std::strong_ordering operator<=>(const signed_number_t &) const;

        signed_number_t &operator++();
        signed_number_t &operator--();
        signed_number_t operator++(int);
        signed_number_t operator--(int);
        signed_number_t operator-();

        signed_number_t &operator+=(const signed_number_t &);
        signed_number_t &operator-=(const signed_number_t &);
        signed_number_t &operator*=(const signed_number_t &);
        signed_number_t &operator/=(signed_number_t);
        signed_number_t &operator%=(signed_number_t);
        signed_number_t &operator<<=(integer_t);
        signed_number_t &operator>>=(integer_t);
    };

    signed_number_t operator+(signed_number_t, const signed_number_t &);
    signed_number_t operator-(signed_number_t, const signed_number_t &);
    signed_number_t operator*(const signed_number_t &, const signed_number_t &);
    signed_number_t operator/(signed_number_t, signed_number_t);
    signed_number_t operator%(signed_number_t, signed_number_t);
    signed_number_t operator<<(signed_number_t, integer_t);
    signed_number_t operator>>(signed_number_t, integer_t);

    std::array<signed_number_t, 2> divide(signed_number_t, signed_number_t);

    void swap(signed_number_t &, signed_number_t &) noexcept;
}

template<>
class std::hash<Machine::unsigned_number_t>
{
public:
    std::size_t operator()(const Machine::unsigned_number_t &arg) const
        { return std::hash<std::vector<Machine::index_t>>{}(arg.digits()); }
};

template<>
class std::hash<Machine::signed_number_t>
{
public:
    std::size_t operator()(const Machine::signed_number_t &arg) const
        { return std::hash<std::vector<Machine::index_t>>{}(arg.abs().digits()); }
};

