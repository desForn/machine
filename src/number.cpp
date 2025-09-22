#include "number.hpp"

#include <utility>
#include <bit>

namespace Machine
{
    static void double_word_addition(index_t &a0, index_t &a1, index_t b)
    {
        static constexpr index_t n_digits = std::numeric_limits<index_t>::digits;
        static_assert(n_digits % 2 == 0);
#if defined(__x86_64__)
        static constexpr bool use_assembly = n_digits == 64;
#else
        static constexpr bool use_assembly = false;
#endif
        if constexpr (use_assembly)
        {
            asm("add %2, %0\n\t"
                "adcq $0x0, %1"
                : "+rm" (a0), "+rm" (a1)
                : "r" (b));

            return;
        }
        else
        {
            a0 += b;
            a1 += a0 < b;

            return;
        }
    }

    static void double_word_subtraction(index_t &a0, index_t &a1, index_t b)
    {
        static constexpr index_t n_digits = std::numeric_limits<index_t>::digits;
        static_assert(n_digits % 2 == 0);
#if defined(__x86_64__)
        static constexpr bool use_assembly = n_digits == 64;
#else
        static constexpr bool use_assembly = false;
#endif
        if constexpr (use_assembly)
        {
            asm("sub %2, %0\n\t"
                "subq $0x0, %1"
                : "+rm" (a0), "+rm" (a1)
                : "r" (b));

            return;
        }
        else
        {
            a1 -= a0 < b;
            a0 -= b;

            return;
        }
    }

    static std::array<index_t, 2> double_word_multiplication(index_t a, index_t b)
    {
        static constexpr index_t n_digits = std::numeric_limits<index_t>::digits;
        static_assert(n_digits % 2 == 0);
#if defined(__x86_64__)
        static constexpr bool use_assembly = n_digits == 64;
#else
        static constexpr bool use_assembly = false;
#endif
        if constexpr (use_assembly)
        {
            index_t c, d;
            asm("mul %2"
                : "=a" (c), "=d" (d)
                : "r" (b), "a" (a));

            return {c, d};
        }

        else
        {
            static constexpr index_t n_digits_half = n_digits / 2;
            static constexpr index_t max = std::numeric_limits<index_t>::max();
            static constexpr std::array<index_t, 2> mask
                {max >> n_digits_half, max << n_digits_half};

            std::array<index_t, 2> ret{0, 0};

            for (index_t i = 0; i != 2; ++i)
            {
                index_t ah = (a & mask[i]) >> (n_digits_half * i);

                index_t carry = 0;
                for (index_t j = 0; j != 2; ++j)
                {
                    index_t bh = (b & mask[j]) >> (n_digits_half * j);
                    index_t rh =
                        (ret[(i + j) / 2] & mask[(i + j) % 2]) >> (n_digits_half * ((i + j) % 2));

                    index_t c = ah * bh + rh + carry;
                    carry = c >> n_digits_half;
                    c &= mask.front();

                    ret[(i + j) / 2] &= mask[(i + j + 1) % 2];
                    ret[(i + j) / 2] |= (c << (n_digits_half * ((i + j) % 2)));
                }

                ret.back() |= (carry << (n_digits_half * i));
            }

            return ret;
        }
    }

    static std::array<index_t, 2> double_word_division(index_t a0, index_t a1, index_t b)
    {
        static constexpr index_t n_digits = std::numeric_limits<index_t>::digits;
        static_assert(n_digits % 2 == 0);
#if defined(__x86_64__)
        static constexpr bool use_assembly = n_digits == 64;
#else
        static constexpr bool use_assembly = false;
#endif

        index_t q, r;
        if constexpr (use_assembly)
        {
            asm("div %4"
                : "=a" (q), "=d" (r)
                : "a" (a0), "d" (a1), "r" (b));

            return {q, r};
        }

        else
        {
            static constexpr index_t n_digits_half = n_digits / 2;
            static constexpr index_t max = std::numeric_limits<index_t>::max();
            static constexpr index_t max_half = max >> n_digits_half;
            static constexpr std::array<index_t, 2> mask{max_half, max << n_digits_half};

            if (a1 == 0)
                return {a0 / b, a0 % b};

            if (a1 >= b)
                throw std::runtime_error{"In Machine::double_word_division(std::array<index_t"
                    ", 2>, index_t):\nOverflow.\n"};

            if (b <= max_half)
            {
                index_t na = (a1 << n_digits_half) | (a0 >> n_digits_half);
                index_t nq = na / b;
                r = na % b;
                q = nq << n_digits_half;

                na = (r << n_digits_half) | (a0 & mask.front());
                nq = na / b;
                r = na % b;
                q |= nq;
            }
            else
            {
                index_t d = n_digits - std::bit_width(b);
                if (d != 0)
                {
                    b <<= d;
                    a1 <<= d;
                    a1 |= (a0 >> (n_digits - d));
                    a0 <<= d;
                }

                const index_t b0 = b & mask.front();
                const index_t b1 = b >> n_digits_half;

                index_t nq, nr;
                if ((a1 >> n_digits_half) == b1)
                {
                    nq = max_half;
                    nr = (a1 & mask.front()) + b1;
                }
                else
                {
                    nq = a1 / b1;
                    nr = a1 % b1;
                }

                index_t mul = nq * b0;
                while (nr <= max_half and mul > ((nr << n_digits_half) | (a0 >> n_digits_half)))
                {
                    --nq;
                    mul -= b0;
                    nr += b1;
                }

                q = nq << n_digits_half;
                index_t na = ((a1 << n_digits_half) | (a0 >> n_digits_half)) - b * nq;

                if ((na >> n_digits_half) == b1)
                {
                    nq = max_half;
                    nr = (na & mask.front()) + b1;
                }
                else
                {
                    nq = na / b1;
                    nr = na % b1;
                }

                mul = nq * b0;
                while (nr <= max_half and mul > ((nr << n_digits_half) | (a0 & mask.front())))
                {
                    --nq;
                    mul -= b0;
                    nr += b1;
                }

                q |= nq;
                r = ((na << n_digits_half) | (a0 & mask.front())) - b * nq;
                r >>= d;
            }
        }

        return {q, r};
    }

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

    const std::vector<index_t> &unsigned_number_t::digits() const noexcept { return digits_; }

    bool unsigned_number_t::zero() const noexcept { return std::empty(digits_); }

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
    {
        std::string ret = b_ary(radix).print_state_reverse(encoder_b_ary_t{radix});
        if (std::empty(ret))
            ret = "0";

        return ret;
    }

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
        { return (digits_[index / 2] & mask[index % 2]) >> (n_digits_half * (index % 2)); }

    void unsigned_number_t::store_half_word(index_t index, index_t arg)
    {
        digits_[index / 2] &= mask[(index + 1) % 2];
        digits_[index / 2] |= (arg << (n_digits_half * (index % 2)));
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
        unsigned_number_t ret{};
        const std::vector<index_t> &va = a.digits_;
        const std::vector<index_t> &vb = b.digits_;
        std::vector<index_t> &vr = ret.digits_;
        index_t sa = std::size(va);
        index_t sb = std::size(vb);
        ret.digits_.resize(sa + sb, 0);

        for (index_t i = 0; i != sa; ++i)
        {
            index_t carry = 0;
            for (index_t j = 0; j != sb; ++j)
            {
                std::array<index_t, 2> c = double_word_multiplication(va[i], vb[j]);
                double_word_addition(c.front(), c.back(), vr[i + j]);
                double_word_addition(c.front(), c.back(), carry);
                
                vr[i + j] = c.front();
                carry = c.back();
            }
            vr[i + sb] = carry;
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
        if (b == 0)
            throw std::runtime_error{"In Machine::divide(unsigned_number_t, index_t):\n"
                "Division by 0.\n"};

        if (a.zero())
            return {};

        std::vector<index_t> &va = a.digits_;
        index_t sa = std::size(va);

        index_t r = 0;
        for (index_t i = sa - 1; i != negative_1; --i)
        {
            index_t c = va[i];
            std::array<index_t, 2> d = double_word_division(c, r, b);

            va[i] = d.front();
            r = d.back();
        }

        a.normalise();

        return {a, r};
    }

    std::array<unsigned_number_t, 2> divide(unsigned_number_t a, unsigned_number_t b)
    {
        static constexpr index_t max = unsigned_number_t::max;
        static constexpr index_t n_digits = unsigned_number_t::n_digits;

        if (b.zero())
            throw std::runtime_error{"In Machine::divide(unsigned_number_t, unsigned_number_t):\n"
                "Division by 0.\n"};

        if (a.zero())
            return {};

        if (a < b)
            return {0, std::move(a)};

        if (std::size(b.digits()) == 1)
            return divide(std::move(a), b.digits().front());

        std::vector<index_t> &va = a.digits_;
        std::vector<index_t> &vb = b.digits_;

        std::array<unsigned_number_t, 2> ret{};
        index_t d, sa, sb;
        {
            d = n_digits - std::bit_width(b.digits().back());

            a <<= d;
            b <<= d;

            sb = std::size(vb);
            b.digits_.push_back(0);

            sa = std::size(va);
            if (sa <= sb or va.back() >= vb.back())
            {
                va.push_back(0);
                ++sa;
            }

            ret.front().digits_.resize(sa - sb);
        }

        const index_t v1 = vb[sb - 1];
        const index_t v2 = vb[sb - 2];

        for (index_t i = sa - sb - 1; i != negative_1; --i)
        {
            index_t q;
            {
                index_t n0 = va[i + sb];
                index_t n1 = va[i + sb - 1];
                index_t n2 = va[i + sb - 2];

                bool overflow_r;
                index_t r;
                if (n0 == v1)
                {
                    q = max;
                    r = n1 + v1;
                    overflow_r = r < v1;
                }
                else
                {
                    std::array<index_t, 2> c = double_word_division(n1, n0, v1);
                    q = c.front();
                    r = c.back();
                    overflow_r = false;
                }

                std::array<index_t, 2> mul = double_word_multiplication(v2, q);
                while (not overflow_r and
                        (mul.back() > r or (mul.back() == r and mul.front() > n2)))
                {
                    --q;
                    double_word_subtraction(mul.front(), mul.back(), v2);
                    r += v1;
                    overflow_r = r < v1;
                }
            }

            if (q != 0)
            {
                index_t mul_carry = 0;
                index_t sub_carry = 0;
                for (index_t j = 0; j != sb + 1; ++j)
                {
                    std::array<index_t, 2> t = double_word_multiplication(q, vb[j]);
                    double_word_addition(t.front(), t.back(), mul_carry);
                    mul_carry = t.back();
                    
                    va[i + j] -= sub_carry;
                    sub_carry = sub_carry and va[i + j] == max;

                    sub_carry = va[i + j] < t.front();
                    va[i + j] -= t.front();
                }

                if (sub_carry)
                {
                    --q;

                    index_t carry = 0;
                    for (index_t j = 0; j != sb + 1; ++j)
                    {
                        va[i + j] += carry;
                        carry = carry and va[i + j] == 0;

                        va[i + j] += vb[j];
                        carry += va[i + j] < vb[j];
                    }
                }
            }

            ret.front().digits_[i] = q;
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
        {
            sign_ = *begin++ == encoder.alphabet().max_character();
            if (*begin >= encoder.alphabet().max_character() - 1)
                throw std::runtime_error{"In Machine::signed_number_t::signed_number_t"
                "(Apparatus::string_iterator_template_t<stride, is_const>, "
                "Apparatus::string_iterator_template_t<stride, is_const>, const encoder_t &):\n"
                "Unexpected sign character.\n"};
        }

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

    const unsigned_number_t &signed_number_t::abs(this const signed_number_t &self)
        { return self.magnitude_; }
    unsigned_number_t signed_number_t::abs(this signed_number_t &&self)
        { return std::move(self.magnitude_); }

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
        if (typeid(encoder) == typeid(encoder_signed_b_ary_t))
            return print_b_ary(encoder.alphabet().max_character() - 1);

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

    signed_number_t signed_number_t::operator+() const { return *this; }

    signed_number_t signed_number_t::operator-() const
    {
        signed_number_t ret {*this};
        ret.sign_ = not sign_;
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

