#include "encoder.hpp"

#include <numeric>

namespace Machine
{
    encoder_t::encoder_t(const alphabet_t &alphabet) noexcept : alphabet_{alphabet} {}

    const alphabet_t &encoder_t::alphabet() const noexcept { return alphabet_; }

    string_t encoder_t::operator()(const std::string &arg) const
    {
        string_t ret{alphabet()};
        ret.resize(std::size(arg));

        auto i = std::begin(ret);
        auto j = std::crbegin(arg);

        for (; i != std::end(ret); ++i, ++j)
            *i = (*this)(*j);

        return ret;
    }

    std::string encoder_t::operator()(const string_t &arg) const
    {
        std::string ret;
        ret.resize(std::size(arg));

        auto i = std::begin(ret);
        auto j = std::crbegin(arg);
        
        for (; i != std::end(ret); ++i, ++j)
            *i = (*this)(*j);

        return ret;
    }

    encoder_ascii_t::encoder_ascii_t() noexcept : encoder_t{255} {}

    encoder_ascii_t *encoder_ascii_t::clone() const { return new encoder_ascii_t{*this}; }

    character_t encoder_ascii_t::operator()(char c) const { return static_cast<character_t>(c); }

    char encoder_ascii_t::operator()(character_t c) const
    {
        if (c > 255)
            throw std::runtime_error{"In Machine::encoder_ascii_t::operator()(character_t) const:\n"
            "Argument is greater than 255.\n"};

        return static_cast<char>(c);
    }

    encoder_list_t::encoder_list_t(std::vector<char> map) :
        encoder_t{alphabet_t{static_cast<character_t>(std::size(map))}},
        map_{std::move(map)}
    {
        static const std::string error{"In Machine::encoder_list_t::encoder_list_t"
            "(std::vector<char>):\n"};

        if (std::empty(map_))
            throw std::runtime_error{error + "Empty map.\n"};

        if (std::size(map_) > std::numeric_limits<character_t>::max())
            throw std::runtime_error{error + "character_t cannot hold all diferent"
                " values of the list.\n"};

        for (character_t i = 0; i != static_cast<character_t>(std::size(map_)); ++i)
        {
            char c = map_[i];
            if (inverse_map_.contains(c))
                throw std::runtime_error{std::string{error + "Character '"} + c +
                    "' is present more than once in the argument.\n"};
            inverse_map_[c] = i;
        }

        return;
    }

    encoder_list_t *encoder_list_t::clone() const { return new encoder_list_t{*this}; }

    character_t encoder_list_t::operator()(char c) const
    {
        auto it = inverse_map_.find(c);

        if (it == std::end(inverse_map_))
            throw std::runtime_error{std::string{"In Machine::encoder_list_t::operator()(char)"
                " const:\nInvalid argument"} + c + ".\n"};

        return it->second;
    }

    char encoder_list_t::operator()(character_t c) const
    {
        if (c >= std::size(map_))
            throw std::runtime_error{std::string{"In Machine::encoder_list_t::operator() const"
                "(character_t):\nInvalid argument"} + std::to_string(c) + ".\n"};
        return map_[c];
    }

    const std::vector<char> &encoder_list_t::list() const { return map_; }
    const std::unordered_map<char, character_t> &encoder_list_t::inverse_map() const
        { return inverse_map_; }

    encoder_alphabetic_t::encoder_alphabetic_t(character_t n) :
        encoder_t{alphabet_t{n ? static_cast<character_t>(n - 1) : static_cast<character_t>(0)}}
    {
        if (n == 0)
            throw std::runtime_error{"In Machine::encoder_alphabetic_t::encoder_alphabetic_t"
                "(character_t):\nThe argument must be non-zero.\n"};

        if (n > 25)
            throw std::runtime_error{"In Machine::encoder_alphabetic_t::encoder_alphabetic_t"
                "(character_t):\nMaximum number of characters in the alphabet is 26.\n"};
        return;
    }

    encoder_alphabetic_t *encoder_alphabetic_t::clone() const
        { return new encoder_alphabetic_t{*this}; }

    character_t encoder_alphabetic_t::operator()(char arg) const
    {
        if (arg < 'a' or arg > 'a' + alphabet().max_character())
            throw std::runtime_error{std::string{"In Machine::encoder_alphabetic_t::operator()"
                "(char) const:\nInvalid argument '"} + arg + "'. Maximum character is '" +
                    std::string{static_cast<char>('a' + alphabet().max_character())} + ".\n"};

        return arg - 'a';
    }

    char encoder_alphabetic_t::operator()(character_t arg) const
    {
        if (arg > alphabet().max_character())
            throw std::runtime_error{std::string{"In Machine::encoder_alphabetic_t::operator()"
                "(character_t) const:\nInvalid argument '"} + std::to_string(arg) +
                    "'. Maximum character is '" +
                    std::to_string(alphabet().max_character()) + ".\n"};

        return static_cast<char>('a' + arg);
    }

    encoder_b_ary_t::encoder_b_ary_t(character_t b) :
        encoder_t{static_cast<character_t>(b - 1)}
    {
        if (b < 2 or b > 36)
            throw std::runtime_error{"In Machine::encoder_b_ary_t::encoder_b_ary_t(character_t)"
                ":\nBase must be between 2 and 36.\n"};
        return;
    }

    encoder_b_ary_t *encoder_b_ary_t::clone() const { return new encoder_b_ary_t{*this}; }

    character_t encoder_b_ary_t::operator()(char c) const
    {
        static const std::string error{"In Machine::encoder_b_ary_t::operator()(char) const:\n"
            "Invalid argument '"};
        if ((c < '0' or c > '9') and (c < 'a' or c > 'z'))
            throw std::runtime_error{error + c + "'. The argument must be alphanumeric.\n"};

        character_t ret;
        if (c >= '0' and c <= '9')
            ret = c - '0';
        else
            ret = c - 'a' + 10;

        const character_t &max_character = alphabet().max_character();

        if (ret > max_character)
        {
            char max;
            if (max_character < 10)
                max = '0' + max_character;
            else
                max = 'a' + max_character - 10;

            throw std::runtime_error{error + c + "'. The maximum character is '" + max + "'.\n"};
        }

        return ret;
    }

    char encoder_b_ary_t::operator()(character_t c) const
    {
        if (c > alphabet().max_character())
            throw std::runtime_error{"In Machine::encoder_b_ary_t::operator()(character_t) const:"
                "\nInvalid argument '" + std::to_string(c) + "'. The maximum character is '" +
                std::to_string(alphabet().max_character()) + "'.\n"};
        if (c < 10)
            return '0' + c;

        return 'a' + c - 10;
    }

    encoder_b_adic_t::encoder_b_adic_t(character_t b) :
        encoder_t{static_cast<character_t>(b - 1)}
    {
        if (b == 0 or b > 35)
            throw std::runtime_error{"In Machine::encoder_b_adic_t::encoder_b_adic_t(character_t)"
                ":\nBase must be between 1 and 35.\n"};
        return;
    }

    encoder_b_adic_t *encoder_b_adic_t::clone() const { return new encoder_b_adic_t{*this}; }

    character_t encoder_b_adic_t::operator()(char c) const
    {
        static const std::string error{"In Machine::encoder_b_adic_t::operator()(char) const:\n"
            "Invalid argument '"};
        if ((c < '1' or c > '9') and (c < 'a' or c > 'z'))
            throw std::runtime_error{error + c + "'. The argument must be alphanumeric.\n"};

        character_t ret;
        if (c >= '1' and c <= '9')
            ret = c - '1';
        else
            ret = c - 'a' + 9;

        const character_t &max_character = alphabet().max_character();

        if (ret > max_character)
        {
            char max;
            if (max_character < 9)
                max = '1' + max_character;
            else
                max = 'a' + max_character - 9;

            throw std::runtime_error{error + c + "'. The maximum character is '" + max + "'.\n"};
        }

        return ret;
    }

    char encoder_b_adic_t::operator()(character_t c) const
    {
        if (c > alphabet().max_character())
            throw std::runtime_error{"In Machine::encoder_b_adic_t::operator()(character_t) const:"
                "\nInvalid argument '" + std::to_string(c) + "'. The maximum character is '" +
                std::to_string(alphabet().max_character()) + "'.\n"};
        if (c < 9)
            return '1' + c;

        return 'a' + c - 9;
    }

    encoder_signed_b_ary_t::encoder_signed_b_ary_t(character_t b) :
        encoder_t{static_cast<character_t>(b + 1)}
    {
        if (b < 2 or b > 36)
            throw std::runtime_error{"In Machine::encoder_signed_b_ary_t::"
                "encoder_signed_b_ary_t(character_t)"
                ":\nBase must be between 2 and 36.\n"};
        return;
    }

    encoder_signed_b_ary_t *encoder_signed_b_ary_t::clone() const
        { return new encoder_signed_b_ary_t{*this}; }

    character_t encoder_signed_b_ary_t::operator()(char c) const
    {
        static const std::string error{"In Machine::encoder_signed_b_ary_t::"
            "operator()(char) const:\nInvalid argument '"};

        const character_t &max_character = alphabet().max_character();

        if (c == '+')
            return max_character - 1;

        if (c == '-')
            return max_character;

        if ((c < '0' or c > '9') and (c < 'a' or c > 'z'))
            throw std::runtime_error{error + c + "'. The argument must be alphanumeric.\n"};

        character_t ret;
        if (c >= '0' and c <= '9')
            ret = c - '0';
        else
            ret = c - 'a' + 10;

        if (ret > max_character)
        {
            char max;
            if (max_character < 10)
                max = '0' + max_character;
            else
                max = 'a' + max_character - 10;

            throw std::runtime_error{error + c + "'. The maximum character is '" + max + "'.\n"};
        }

        return ret;
    }

    char encoder_signed_b_ary_t::operator()(character_t c) const
    {
        const character_t max_character = alphabet().max_character();

        if (c > max_character)
            throw std::runtime_error{"In Machine::encoder_signed_b_ary_t::operator()(character_t) const:"
                "\nInvalid argument '" + std::to_string(c) + "'. The maximum character is '" +
                std::to_string(alphabet().max_character()) + "'.\n"};

        if (c == max_character - 1)
            return '+';

        if (c == max_character)
            return '-';

        if (c < 10)
            return '0' + c;

        return 'a' + c - 10;
    }

    encoder_signed_b_adic_t::encoder_signed_b_adic_t(character_t b) :
        encoder_t{static_cast<character_t>(b + 1)}
    {
        if (b == 0 or b > 35)
            throw std::runtime_error{"In Machine::encoder_signed_b_adic_t::"
                "encoder_signed_b_adic_t(character_t):\n"
                "Base must be between 1 and 35.\n"};
        return;
    }

    encoder_signed_b_adic_t *encoder_signed_b_adic_t::clone() const
        { return new encoder_signed_b_adic_t{*this}; }

    character_t encoder_signed_b_adic_t::operator()(char c) const
    {
        static const std::string error{"In Machine::encoder_signed_b_adic_t::"
            "operator()(char) const:\nInvalid argument '"};

        const character_t &max_character = alphabet().max_character();

        if (c == '+')
            return max_character - 1;

        if (c == '-')
            return max_character;

        if ((c < '1' or c > '9') and (c < 'a' or c > 'z'))
            throw std::runtime_error{error + c + "'. The argument must be alphanumeric.\n"};

        character_t ret;
        if (c >= '1' and c <= '9')
            ret = c - '1';
        else
            ret = c - 'a' + 9;


        if (ret > max_character)
        {
            char max;
            if (max_character < 9)
                max = '1' + max_character;
            else
                max = 'a' + max_character - 9;

            throw std::runtime_error{error + c + "'. The maximum character is '" + max + "'.\n"};
        }

        return ret;
    }

    char encoder_signed_b_adic_t::operator()(character_t c) const
    {
        const character_t &max_character = alphabet().max_character();

        if (c > max_character)
            throw std::runtime_error{"In Machine::encoder_signed_b_adic_t::operator()(character_t)"
                " const:\nInvalid argument '" + std::to_string(c) + "'. The maximum character is '"
                    + std::to_string(alphabet().max_character()) + "'.\n"};

        if (c == max_character - 1)
            return '+';

        if (c == max_character)
            return '-';

        if (c < 9)
            return '1' + c;

        return 'a' + c - 9;
    }

    encoder_separator_t::encoder_separator_t(const encoder_separator_t &arg) :
        encoder_t{arg.alphabet().max_character()},
        extended_encoder_{std::unique_ptr<encoder_t>{arg.extended_encoder_->clone()}},
        separator_{arg.separator_} {}

    encoder_separator_t &encoder_separator_t::operator=(const encoder_separator_t &)
        { return *this = encoder_separator_t{*this}; }

    encoder_separator_t::encoder_separator_t(std::unique_ptr<encoder_t> encoder, char separator) :
        encoder_t{encoder ? static_cast<character_t>(encoder->alphabet().max_character() + 1) :
            static_cast<character_t>(0)},
        extended_encoder_{std::move(encoder)},
        separator_{separator}
    {
        std::string error_prefix = "In Machine::encoder_separator_t::encoder_separator_t(std::"
            "unique_ptr<encoder_t>, char):\n";

        if (not extended_encoder_)
            throw std::runtime_error{error_prefix + "Null parent encoder.\n"};

        if (extended_encoder_->alphabet().max_character() ==
                std::numeric_limits<character_t>::max())
            throw std::runtime_error{error_prefix + "The alphabet is too large for the "
                "Machine::character_t type.\n"};

        try
        {
            static_cast<void>((*extended_encoder_)(separator_));

            throw std::runtime_error{error_prefix + "Invalid separator; already present in the"
                "parent encoder.\n"};
        }

        catch (std::runtime_error &) {}

        return;
    }

    encoder_separator_t *encoder_separator_t::clone() const
        { return new encoder_separator_t{*this}; }

    character_t encoder_separator_t::operator()(char c) const
    {
        if (c == separator_)
            return alphabet().max_character();

        return (*extended_encoder_)(c);
    }

    char encoder_separator_t::operator()(character_t c) const
    {
        if (c == alphabet().max_character())
            return separator_;

        return (*extended_encoder_)(c);
    }

    char encoder_separator_t::separator() const { return separator_; }
}

