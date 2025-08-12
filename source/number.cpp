#include "number.hpp"

#include <utility>
#include <bit>
#include <cassert>
#include <iostream>

namespace Machine
{
    unsigned_number_t::unsigned_number_t(index_t num)
    {
        if (num != 0)
            digits_.push_back(num);

        return;
    }

    unsigned_number_t &unsigned_number_t::operator=(index_t num)
        { return *this = unsigned_number_t{num}; }

    template<integer_t stride, bool is_const>
    unsigned_number_t::unsigned_number_t(
            Apparatus::string_iterator_template_t<stride, is_const> begin,
            Apparatus::string_iterator_template_t<stride, is_const> end, const encoder_t &encoder)
    {
        static const std::string error{"In Machine::unsigned_number_t::unsigned_number_t"
                "(Apparatus::string_iterator_template_t<stride, is_const>, "
                "Apparatus::string_iterator_template_t<stride, is_const>, const encoder_t &):\n"};

        if (typeid(encoder) == typeid(encoder_b_ary_t))
        {
            const index_t radix = encoder.alphabet().max_character() + 1;

            for (; begin != end; ++begin)
                *this = *this * radix + *begin;
        }
        else if (typeid(encoder) == typeid(encoder_b_adic_t))
        {
            const index_t radix = encoder.alphabet().max_character() + 1;

            for (; begin != end; ++begin)
                *this = *this * radix + *begin + static_cast<character_t>(1);
        }
        else if (typeid(encoder) == typeid(encoder_signed_b_ary_t))
        {
            const index_t radix = encoder.alphabet().max_character() - 1;

            if (*begin == radix)
                ++begin;

            for (; begin != end; ++begin)
            {
                if (*begin >= radix)
                    throw std::runtime_error{error + "Unexpected sign character.\n"};
                *this = *this * radix + *begin;
            }
        }
        else if (typeid(encoder) == typeid(encoder_signed_b_adic_t))
        {
            const index_t radix = encoder.alphabet().max_character() - 1;

            if (*begin == radix)
                ++begin;

            for (; begin != end; ++begin)
            {
                if (*begin >= radix)
                    throw std::runtime_error{error + "Unexpected sign character.\n"};
                *this = *this * radix + *begin + static_cast<character_t>(1);
            }
        }
        else
            throw std::runtime_error{error + "Unknown encoder type.\n"};

        return;        
    }

    unsigned_number_t::unsigned_number_t(std::string_view string, const encoder_t &encoder)
    {
        static const std::string error{"In Machine::unsigned_number_t::unsigned_number_t"
                "(std::string_view, const encoder_t &):\n"};

        if (typeid(encoder) == typeid(encoder_b_ary_t))
        {
            const index_t radix = encoder.alphabet().max_character() + 1;

            for (char c : string)
                *this = *this * radix + encoder(c);
        }
        else if (typeid(encoder) == typeid(encoder_b_adic_t))
        {
            const index_t radix = encoder.alphabet().max_character() + 1;

            for (char c : string)
                *this = *this * radix + encoder(c) + static_cast<character_t>(1);
        }
        else if (typeid(encoder) == typeid(encoder_signed_b_ary_t))
        {
            const index_t radix = encoder.alphabet().max_character() - 1;

            if (string.front() == '+')
                string.remove_prefix(1);

            for (char c : string)
            {
                character_t d = encoder(c);

                if (d >= radix)
                    throw std::runtime_error{error + "Unexpected sign character.\n"};

                *this = *this * radix + d;
            }
        }
        else if (typeid(encoder) == typeid(encoder_signed_b_adic_t))
        {
            const index_t radix = encoder.alphabet().max_character() - 1;

            if (string.front() == '+')
                string.remove_prefix(1);

            for (char c : string)
            {
                character_t d = encoder(c);

                if (d >= radix)
                    throw std::runtime_error{error + "Unexpected sign character.\n"};

                *this = *this * radix + d + static_cast<character_t>(1);
            }
        }
        else
            throw std::runtime_error{error + "Unknown encoder type.\n"};

        return;        
    }

    void unsigned_number_t::swap(unsigned_number_t &arg) noexcept { digits_.swap(arg.digits_); }

    const std::vector<index_t> &unsigned_number_t::digits() const { return digits_; }

    bool unsigned_number_t::zero() const { return std::empty(digits_); }

    string_t unsigned_number_t::b_ary(character_t radix) const
    {
        string_t ret{radix};

        for (unsigned_number_t a = *this; not a.zero();)
        {
            std::array<unsigned_number_t, 2> b = divide(std::move(a), radix);
            unsigned_number_t &r = b.back();
            a = b.front();
            if (r.zero())
                ret.push(0);
            else
                ret.push(static_cast<character_t>(r.digits().front()));
        }

        return ret;
    }

    string_t unsigned_number_t::b_adic(character_t radix) const
    {
        string_t ret{radix};

        for (unsigned_number_t a = *this; not a.zero();)
        {
            std::array<unsigned_number_t, 2> b = divide(std::move(a), radix);
            unsigned_number_t &r = b.back();
            a = b.front();
            if (r.zero())
            {
                ret.push(radix - 1);
                --a;
            }
            else
                ret.push(static_cast<character_t>(r.digits().front() - 1));
        }

        return ret;
    }

    std::string unsigned_number_t::print_b_ary(character_t radix) const
        { return b_ary(radix).print_state_reverse(encoder_b_ary_t{radix}); }

    std::string unsigned_number_t::print_b_adic(character_t radix) const
        { return b_adic(radix).print_state_reverse(encoder_b_adic_t{radix}); }

    std::string unsigned_number_t::print(const encoder_t &encoder) const
    {
        if (typeid(encoder) == typeid(encoder_b_ary_t))
            return print_b_ary(encoder.alphabet().max_character() + 1);
        
        if (typeid(encoder) == typeid(encoder_signed_b_ary_t))
            return print_b_ary(encoder.alphabet().max_character() - 1);

        if (typeid(encoder) == typeid(encoder_b_adic_t))
            return print_b_adic(encoder.alphabet().max_character() + 1);

        if (typeid(encoder) == typeid(encoder_signed_b_adic_t))
            return print_b_adic(encoder.alphabet().max_character() - 1);

        else
            throw std::runtime_error{"In Machine::unsigned_number_t::print(const encoder_t &):\n"
                "Unknown encoder type.\n"};
    }

    index_t unsigned_number_t::load_half_word(index_t index) const
        { return (digits_[index / 2] & mask[index % 2]) >> (n_digits / 2 * (index % 2)); }

    void unsigned_number_t::store_half_word(index_t index, index_t arg)
    {
        digits_[index / 2] &= mask[(index + 1) % 2];
        digits_[index / 2] |= (arg << (n_digits / 2 * (index % 2)));
        return;
    }

    void unsigned_number_t::normalise()
    {
        while (not std::empty(digits_) and digits_.back() == 0)
            digits_.pop_back();
        return;
    }

    std::strong_ordering unsigned_number_t::operator<=>(const unsigned_number_t &arg) const
    {
        if (std::size(digits_) != std::size(arg.digits_))
            return std::size(digits_) <=> std::size(arg.digits_);

        for (auto i = std::crbegin(digits_), j = std::crbegin(arg.digits_); i != crend(digits_);
                ++i, ++j)
            if (*i != *j)
                return *i <=> *j;

        return std::strong_ordering::equivalent;
    }

    unsigned_number_t &unsigned_number_t::operator++()
    {
        for (auto i = std::begin(digits_); i != std::end(digits_); ++i)
        {
            if (*i != max)
            {
                ++*i;
                return *this;
            }

            *i = 0;
        }

        digits_.push_back(1);
        return *this;
    }

    unsigned_number_t &unsigned_number_t::operator--()
    {
        if (zero())
            throw std::runtime_error{"In Machine::unsigned_number_t::operator--():\n"
                "Argument is 0.\n"};

        for (auto i = std::begin(digits_); i != std::end(digits_); ++i)
        {
            if (*i != 0)
            {
                --*i;

                if (*i == 0 and i == std::cend(digits_) - 1)
                    digits_.pop_back();

                return *this;
            }

            *i = max;
        }

        std::unreachable();
    }

    unsigned_number_t unsigned_number_t::operator++(int)
    {
        unsigned_number_t ret{*this};
        ++*this;
        return ret;
    }

    unsigned_number_t unsigned_number_t::operator--(int)
    {
        unsigned_number_t ret{*this};
        --*this;
        return ret;
    }

    unsigned_number_t &unsigned_number_t::operator+=(const unsigned_number_t &arg)
    {
        auto i = std::begin(digits_);
        auto j = std::cbegin(arg.digits_);
        bool carry = false;

        for (; i != std::end(digits_) and j != std::cend(arg.digits_); ++i, ++j)
        {
            *i += *j;
            bool next_carry = *i < *j;

            if (carry)
            {
                ++*i;
                next_carry = next_carry or (*i == 0);
            }

            carry = next_carry;
        }

        index_t d = std::distance(std::begin(digits_), i);

        digits_.append_range(std::ranges::subrange(j, std::cend(arg.digits_)));

        i = std::begin(digits_) + d;

        for (; carry and i != std::cend(digits_); ++i)
        {
            ++*i;
            carry = (*i == 0);
        }

        if (carry)
            digits_.push_back(1);

        return *this;
    }

    unsigned_number_t &unsigned_number_t::operator-=(const unsigned_number_t &arg)
    {
        if (*this < arg)
            throw std::runtime_error{"In Machine::unsigned_number_t::operator-=(const "
                "unsigned_number_t &) const:\nThe result would be negative.\n"};

        auto i = std::begin(digits_);
        auto j = std::cbegin(arg.digits_);
        bool carry = false;

        for (; i != std::end(digits_) and j != std::cend(arg.digits_); ++i, ++j)
        {
            bool next_carry = *i < *j;
            *i -= *j; 

            if (carry)
            {
                --*i;
                next_carry = next_carry or (*i == max);
            }

            carry = next_carry;
        }

        if (carry)
            for (; i != std::cend(digits_); ++i)
            {
                if (*i == 0)
                    *i = max;
                else
                {
                    --*i;
                    break;
                }
            } 

        index_t new_size = std::size(digits_);

        for (auto i = std::crbegin(digits_); *i == 0 and i != std::crend(digits_); ++i)
            --new_size;

        digits_.resize(new_size);

        return *this;
    }

    unsigned_number_t &unsigned_number_t::operator*=(const unsigned_number_t &arg)
        { return *this = *this * arg; }

    unsigned_number_t &unsigned_number_t::operator/=(unsigned_number_t arg)
        { return *this = divide(std::move(*this), std::move(arg)).front(); }

    unsigned_number_t &unsigned_number_t::operator%=(unsigned_number_t arg)
        { return *this = divide(std::move(*this), std::move(arg)).back(); }

    unsigned_number_t &unsigned_number_t::operator/=(index_t arg)
        { return *this = divide(std::move(*this), arg).front(); }

    unsigned_number_t &unsigned_number_t::operator%=(index_t arg)
        { return *this = divide(std::move(*this), arg).back(); }

    unsigned_number_t &unsigned_number_t::operator<<=(integer_t arg)
    {
        if (arg < 0)
            return *this >>= -arg;

        const index_t a = arg / n_digits;
        const index_t b = arg % n_digits;

        if (zero())
            return *this;

        if (b != 0)
        {
            const index_t mask = max << (n_digits - b);

            index_t c = 0;

            for (auto &i : digits_)
            {
                index_t d = (i & mask) >> (n_digits - b);
                i <<= b;
                i |= c;
                c = d;
            }

            if (c != 0)
                digits_.emplace_back(c);
        }

        digits_.resize(std::size(digits_) + a);
        
        auto i = std::rbegin(digits_);
        for (auto  j = std::rbegin(digits_) + a; j != std::rend(digits_); ++i, ++j)
            *i = *j;
        for (; i != std::rend(digits_); ++i)
            *i = 0;

        return *this;
    }

    unsigned_number_t &unsigned_number_t::operator>>=(integer_t arg)
    {
        if (arg < 0)
            return *this <<= -arg;

        const index_t a = arg / n_digits;
        const index_t b = arg % n_digits;

        if (a >= std::size(digits_))
        {
            digits_.clear();
            return *this;
        }

        else
            digits_.erase(std::begin(digits_), std::begin(digits_) + a);

        if (b != 0)
        {
            const index_t mask = max >> (n_digits - b);

            index_t c = 0;

            for (auto i = std::rbegin(digits_); i != std::rend(digits_); ++i)
            {
                index_t d = (*i & mask) << (n_digits - b);
                *i >>= b;
                *i |= c;
                c = d;
            }

            if (digits_.back() == 0)
                digits_.pop_back();
        }

        return *this;
    }

    unsigned_number_t operator+(unsigned_number_t arg0, const unsigned_number_t &arg1)
        { return arg0 += arg1; }

    unsigned_number_t operator-(unsigned_number_t arg0, const unsigned_number_t &arg1)
        { return arg0 -= arg1; }

    unsigned_number_t operator*(const unsigned_number_t &a, const unsigned_number_t &b)
    {
        static constexpr index_t n_digits_half = unsigned_number_t::n_digits_half;
        static constexpr std::array<index_t, 2> mask = unsigned_number_t::mask;

        unsigned_number_t ret{};
        index_t sa = std::size(a.digits_) * 2;
        index_t sb = std::size(b.digits_) * 2;
        ret.digits_.resize((sa + sb) * 2, 0);

        for (index_t i = 0; i != sa; ++i)
        {
            index_t carry = 0;
            for (index_t j = 0; j != sb; ++j)
            {
                index_t na = a.load_half_word(i);
                index_t nb = b.load_half_word(j);
                index_t nr = ret.load_half_word(i + j);

                index_t t = na * nb + nr + carry;
                carry = t >> n_digits_half;
                ret.store_half_word(i + j, t & mask.front());
            }

            ret.store_half_word(i + sb, carry);
        }

        ret.normalise();

        return ret;
    }

    unsigned_number_t operator/(unsigned_number_t arg0, unsigned_number_t arg1)
        { return arg0 /= std::move(arg1); }

    unsigned_number_t operator%(unsigned_number_t arg0, unsigned_number_t arg1)
        { return arg0 %= std::move(arg1); }

    unsigned_number_t operator/(unsigned_number_t arg0, integer_t arg1)
        { return arg0 /= arg1; }

    unsigned_number_t operator%(unsigned_number_t arg0, integer_t arg1)
        { return arg0 %= arg1; }

    unsigned_number_t operator<<(unsigned_number_t arg0, integer_t arg1)
        { return arg0 <<= arg1; }

    unsigned_number_t operator>>(unsigned_number_t arg0, integer_t arg1)
        { return arg0 >>= arg1; }

    std::array<unsigned_number_t, 2> divide(unsigned_number_t a, index_t b)
    {
        static constexpr index_t max_half = unsigned_number_t::max_half;
        static constexpr index_t n_digits_half = unsigned_number_t::n_digits_half;

        if (b == 0)
            throw std::runtime_error{"In Machine::divide(unsigned_number_t, index_t):\n"
                "Division by 0.\n"};

        if (a.zero())
            return {};

        if (b > max_half)
            return divide(std::move(a), unsigned_number_t{b});

        index_t sa = std::size(a.digits_) * 2;

        index_t r = 0;
        for (index_t i = sa - 1; i != negative_1; --i)
        {
            index_t na = a.load_half_word(i) + (r << n_digits_half);
            index_t nr = na / b;
            r = na % b;

            a.store_half_word(i, nr);
        }

        a.normalise();
        return {a, r};
    }

    std::array<unsigned_number_t, 2> divide(unsigned_number_t a, unsigned_number_t b)
    {
        static constexpr index_t max_half = unsigned_number_t::max_half;
        static constexpr index_t n_digits = unsigned_number_t::n_digits;
        static constexpr index_t n_digits_half = unsigned_number_t::n_digits_half;
        static constexpr std::array<index_t, 2> mask = unsigned_number_t::mask;

        if (b.zero())
            throw std::runtime_error{"In Machine::divide(unsigned_number_t, unsigned_number_t):\n"
                "Division by 0.\n"};

        if (a.zero())
            return {};

        if (std::size(b.digits()) == 1 and b.digits().front() <= max_half)
            return divide(std::move(a), b.digits().front());

        if (a < b)
            return {0, std::move(a)};

        std::array<unsigned_number_t, 2> ret{};
        index_t d, sa, sb;
        {
            d = n_digits - std::bit_width(b.digits().back());
            bool half_word_shift_b = d >= n_digits_half;
            if (half_word_shift_b)
                d -= n_digits_half;

            a <<= d;
            b <<= d;

            sb = std::size(b.digits_) * 2 - half_word_shift_b;
            b.digits_.push_back(0);

            bool half_word_shift_a = a.digits_.back() <= max_half;
            sa = std::size(a.digits_) * 2 - half_word_shift_a;
            if (sa <= sb or a.digits_.back() >= b.digits_.back())
            {
                if (not half_word_shift_a)
                    a.digits_.push_back(0);
                ++sa;
            }

            ret.front().digits_.resize((sa - sb + 1) / 2);
        }

        const index_t v1 = b.load_half_word(sb - 1);
        const index_t v2 = b.load_half_word(sb - 2);

        for (index_t i = sa - sb - 1; i != negative_1; --i)
        {
            index_t q;
            {
                index_t n0 = a.load_half_word(i + sb);
                index_t n1 = a.load_half_word(i - 1 + sb);
                index_t n2 = a.load_half_word(i - 2 + sb);

                index_t r;
                if (n0 == v1)
                {
                    q = max_half;
                    r = n1 + v1;
                }
                else
                {
                    index_t c = (n0 << n_digits_half) + n1;
                    q = c / v1;
                    r = q % v1;
                }

                index_t mul = v2 * q;
                while (r <= max_half and mul > (r << n_digits_half) + n2)
                {
                    --q;
                    mul -= v2;
                    r += v1;
                }
            }

            if (q != 0)
            {
                index_t mul_carry = 0;
                bool sub_carry = false;
                for (index_t j = 0; j != sb + 1; ++j)
                {
                    index_t u = a.load_half_word(i + j);
                    index_t v = b.load_half_word(j);

                    index_t t = q * v + mul_carry;
                    mul_carry = t >> n_digits_half;
                    t &= mask.front();

                    bool next_sub_carry = u < t;
                    u = (u - t) & mask.front();

                    if (sub_carry)
                    {
                        if (u == 0)
                        {
                            u = max_half;
                            next_sub_carry = true;
                        }
                        else
                            --u;
                    }
                    sub_carry = next_sub_carry;

                    a.store_half_word(i + j, u);
                }

                if (sub_carry)
                {
                    --q;

                    index_t carry = 0;
                    for (index_t j = 0; j != sb + 1; ++j)
                    {
                        index_t u = a.load_half_word(i + j);
                        index_t v = b.load_half_word(j);

                        u += v + carry;
                        carry = u >> n_digits_half;

                        a.store_half_word(i + j, u & mask.front());
                    }
                }
            }

            ret.front().store_half_word(i, q);
        }

        ret.front().normalise();
        a.normalise();

        ret.back() = std::move(a) >> d;
        return ret;
    }

    void swap(unsigned_number_t &arg0, unsigned_number_t &arg1) noexcept { arg0.swap(arg1); }

    signed_number_t::signed_number_t(integer_t arg) :
        sign_{arg < 0}, magnitude_{static_cast<index_t>(std::abs(arg))} {}
    signed_number_t &signed_number_t::operator=(integer_t arg)
        { return *this = signed_number_t{arg}; }

    signed_number_t::signed_number_t(unsigned_number_t arg) :
        sign_{pos_}, magnitude_{std::move(arg)} {}
    signed_number_t &signed_number_t::operator=(unsigned_number_t arg)
        { return *this = signed_number_t{arg}; }

    template<integer_t stride, bool is_const>
    signed_number_t::signed_number_t(
        Apparatus::string_iterator_template_t<stride, is_const> begin,
        Apparatus::string_iterator_template_t<stride, is_const> end, const encoder_t &encoder)
    {
        if ((typeid(encoder) == typeid(encoder_signed_b_ary_t) or
                typeid(encoder) == typeid(encoder_signed_b_adic_t)) and
                *begin >= encoder.alphabet().max_character() - 1)
            sign_ = *begin++ == encoder.alphabet().max_character();

        magnitude_ = unsigned_number_t{begin, end, encoder};

        return;
    }

    signed_number_t::signed_number_t(std::string_view string, const encoder_t &encoder)
    {
        if (string.front() == '+')
        {
            sign_ = pos_;
            string.remove_prefix(1);
        }
        else if (string.front() == '-')
        {
            sign_ = neg_;
            string.remove_prefix(1);
        }

        magnitude_ = unsigned_number_t{string, encoder};

        return;
    }

    void signed_number_t::swap(signed_number_t &arg) noexcept
    {
        std::swap(sign_, arg.sign_);
        std::swap(magnitude_, arg.magnitude_);
        return;
    }

    const unsigned_number_t &signed_number_t::abs() const & { return magnitude_; }
    unsigned_number_t signed_number_t::abs() && { return std::move(magnitude_); }

    bool signed_number_t::zero() const { return abs().zero(); }
    bool signed_number_t::pos() const { return not zero() and sign_ == pos_; }
    bool signed_number_t::neg() const { return not zero() and sign_ == neg_; }

    string_t signed_number_t::b_ary(character_t c) const
    {
        string_t m = magnitude_.b_ary(c);
        
        if (not neg())
            return m;

        string_t ret{++c};
        ret.push(c);

        for (character_t a : m)
            ret.push(a);

        return ret;
    }

    string_t signed_number_t::b_adic(character_t c) const
    {
        string_t m = magnitude_.b_adic(c);

        if (not neg())
            return m;

        string_t ret{++c};
        ret.push(c);

        for (character_t a : m)
            ret.push(a);

        return ret;
    }

    std::string signed_number_t::print_b_ary(character_t c) const
    {
        std::string m = magnitude_.print_b_ary(c);

        if (not neg())
            return m;

        std::string ret{"-"};

        for (char a : m)
            ret.push_back(a);

        return ret;
    }

    std::string signed_number_t::print_b_adic(character_t c) const
    {
        std::string m = magnitude_.print_b_adic(c);

        if (not neg())
            return m;

        std::string ret{"-"};

        for (char a : m)
            ret.push_back(a);

        return ret;
    }

    std::string signed_number_t::print(const encoder_t &encoder) const
    {
        if (typeid(encoder) == typeid(encoder_b_ary_t))
            return print_b_ary(encoder.alphabet().max_character() + 1);
        
        if (typeid(encoder) == typeid(encoder_signed_b_ary_t))
            return print_b_ary(encoder.alphabet().max_character() - 1);

        if (typeid(encoder) == typeid(encoder_b_adic_t))
            return print_b_adic(encoder.alphabet().max_character() + 1);

        if (typeid(encoder) == typeid(encoder_signed_b_adic_t))
            return print_b_adic(encoder.alphabet().max_character() - 1);

        else
            throw std::runtime_error{"In Machine::unsigned_number_t::print(const encoder_t &):\n"
                "Unknown encoder type.\n"};
    }

    bool signed_number_t::operator==(const signed_number_t &arg) const
    {
        if (zero() and arg.zero())
            return true;
        
        if (sign_ != arg.sign_)
            return false;

        return magnitude_ == arg.magnitude_;
    }

    bool signed_number_t::operator!=(const signed_number_t &arg) const
        { return not (*this == arg); }

    std::strong_ordering signed_number_t::operator<=>(const signed_number_t &arg) const
    {
        if (zero() and arg.zero())
            return std::strong_ordering::equivalent;

        if (sign_ != arg.sign_)
        {
            if(sign_ == neg_)
                return std::strong_ordering::less;
            return std::strong_ordering::greater;
        }

        if (sign_ == pos_)
            return magnitude_ <=> arg.magnitude_;
        return arg.magnitude_ <=> magnitude_;
    }

    signed_number_t &signed_number_t::operator++()
    {
        if (zero())
            sign_ = pos_;

        if (sign_ == pos_)
            ++magnitude_;
        else
            --magnitude_;
        return *this;
    }

    signed_number_t &signed_number_t::operator--()
    {
        if (zero())
            sign_ = neg_;

        if (sign_ == pos_)
            --magnitude_;
        else
            ++magnitude_;
        return *this;
    }

    signed_number_t signed_number_t::operator++(int)
    {
        signed_number_t ret{*this};
        ++*this;
        return ret;
    }

    signed_number_t signed_number_t::operator--(int)
    {
        signed_number_t ret{*this};
        --*this;
        return ret;
    }

    signed_number_t signed_number_t::operator-()
    {
        signed_number_t ret {*this};
        sign_ = not sign_;
        return ret;
    }

    signed_number_t &signed_number_t::operator+=(const signed_number_t &arg)
    {
        if (sign_ == arg.sign_)
            magnitude_ += arg.magnitude_;
        else
        {
            if (magnitude_ >= arg.magnitude_)
                magnitude_ -= arg.magnitude_;
            else
            {
                magnitude_ = arg.magnitude_ - magnitude_;
                sign_ = arg.sign_;
            }
        }

        return *this;
    }

    signed_number_t &signed_number_t::operator-=(const signed_number_t &arg)
    {
        if (sign_ != arg.sign_)
            magnitude_ += arg.magnitude_;
        else
        {
            if (magnitude_ >= arg.magnitude_)
                magnitude_ -= arg.magnitude_;
            else
            {
                magnitude_ = arg.magnitude_ - magnitude_;
                sign_ = not arg.sign_;
            }
        }

        return *this;
    }

    signed_number_t &signed_number_t::operator*=(const signed_number_t &arg)
    {
        sign_ = sign_ != arg.sign_;
        magnitude_ *= arg.magnitude_;
        return *this;
    }

    signed_number_t &signed_number_t::operator/=(signed_number_t arg)
        { return *this = divide(std::move(*this), std::move(arg)).front(); }

    signed_number_t &signed_number_t::operator%=(signed_number_t arg)
        { return *this = divide(std::move(*this), std::move(arg)).back(); }

    signed_number_t &signed_number_t::operator<<=(integer_t arg)
    {
        magnitude_ <<= arg;
        return *this;
    }

    signed_number_t &signed_number_t::operator>>=(integer_t arg)
    {
        magnitude_ >>= arg;
        return *this;
    }

    signed_number_t operator+(signed_number_t arg0, const signed_number_t &arg1)
        { return arg0 += arg1; }

    signed_number_t operator-(signed_number_t arg0, const signed_number_t &arg1)
        { return arg0 -= arg1; }

    signed_number_t operator*(signed_number_t arg0, const signed_number_t &arg1)
        { return arg0 *= arg1; }

    signed_number_t operator/(signed_number_t arg0, signed_number_t arg1)
        { return arg0 /= std::move(arg1); }

    signed_number_t operator%(signed_number_t arg0, signed_number_t arg1)
        { return arg0 %= std::move(arg1); }

    signed_number_t operator<<(signed_number_t arg0, integer_t arg1)
        { return arg0 <<= arg1; }

    signed_number_t operator>>(signed_number_t arg0, integer_t arg1)
        { return arg0 >>= arg1; }

    std::array<signed_number_t, 2> divide(signed_number_t a, signed_number_t b)
    {
        bool negative_a = a.neg();
        bool negative_b = b.neg();

        std::array<unsigned_number_t, 2> unsigned_ret =
            divide(std::move(a).abs(), b.abs());
        std::array<signed_number_t, 2> ret
            {std::move(unsigned_ret.front()), std::move(unsigned_ret.back())};

        signed_number_t &q = ret.front();
        signed_number_t &r = ret.back();

        if (not negative_a and negative_b)
        {
            q.sign_ = true;

            if (not r.zero())
            {
                --q;
                r += b;
            }
        }

        if (negative_a and not negative_b)
        {
            q.sign_ = true;
            r.sign_ = true;

            if (not r.zero())
            {
                --q;
                r += b;
            }
        }

        if (negative_a and negative_b)
        {
            r.sign_ = true;
        }

        return ret;
    }

    void swap(signed_number_t &arg0, signed_number_t &arg1) noexcept { arg0.swap(arg1); }
}

