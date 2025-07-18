#include "encoder.hpp"

#include <numeric>

namespace Machine
{
    encoder_t::encoder_t(const alphabet_t &alphabet) : alphabet_{alphabet} {}

    const alphabet_t &encoder_t::alphabet() const { return alphabet_; }

    string_t encoder_t::operator()(const std::string &arg) const
    {
        string_t ret{alphabet()};

        for (auto it = std::crbegin(arg); it != std::crend(arg) ; ++it)
            ret.push((*this)(*it));

        return ret;
    }

    std::string encoder_t::operator()(const string_t &arg) const
    {
        std::string ret;

        for (auto it = std::crbegin(arg.data()); it != std::crend(arg.data()); ++it)
            ret.push_back((*this)(*it));

        return ret;
    }

    encoder_ascii_t::encoder_ascii_t() : encoder_t{256} {}

    encoder_ascii_t *encoder_ascii_t::clone() const { return new encoder_ascii_t{*this}; }

    character_t encoder_ascii_t::operator()(char c) const { return static_cast<character_t>(c); }

    char encoder_ascii_t::operator()(character_t c) const
    {
        if (c > 255)
            throw std::runtime_error{"In encoder_ascii_t::operator()(character_t)."};

        return static_cast<char>(c);
    }

    encoder_list_t::encoder_list_t(std::vector<char> map) :
        encoder_t{alphabet_t{static_cast<character_t>(std::size(map))}},
        map_{std::move(map)}
    {
        static const std::runtime_error error{"In encoder_list_t::encoder_list_t(const alphabet_t &"
            "std::vector<char>)."};

        if (std::size(map_) > std::numeric_limits<character_t>::max())
            throw error;

        for (character_t i = 0; i != static_cast<character_t>(std::size(map_)); ++i)
        {
            char c = map_[i];
            if (inverse_map_.contains(c))
                throw error;
            inverse_map_[c] = i;
        }

        return;
    }

    encoder_list_t *encoder_list_t::clone() const { return new encoder_list_t{*this}; }

    character_t encoder_list_t::operator()(char c) const
    {
        auto it = inverse_map_.find(c);

        if (it == std::end(inverse_map_))
            throw std::runtime_error{"In encoder_list_t::operator()(char)."};

        return it->second;
    }

    char encoder_list_t::operator()(character_t c) const
    {
        if (c >= std::size(map_))
            throw std::runtime_error{"In encoder_list_t::operator()(character_t)."};
        return map_[c];
    }

    const std::vector<char> &encoder_list_t::list() const { return map_; }
    const std::unordered_map<char, character_t> &encoder_list_t::inverse_map() const
        { return inverse_map_; }

    encoder_alphabetic_t::encoder_alphabetic_t(character_t n) : encoder_t{alphabet_t{n}}
    {
        if (n > 26)
            throw std::runtime_error{"In encoder_alphabetic_t::encoder_alphabetic_t(index_t).\n"
                "Maximum number of characters in the alphabet is 26."};
        return;
    }

    encoder_alphabetic_t::encoder_alphabetic_t(const alphabet_t &alphabet) : encoder_t{alphabet}
    {
        if (alphabet_.n_characters() > 26)
            throw std::runtime_error{"In encoder_alphabetic_t::encoder_alphabetic_t"
                "(const alphabet_t &).\n"
                "Maximum number of characters in the alphabet is 26."};
        return;
    }

    encoder_alphabetic_t *encoder_alphabetic_t::clone() const
        { return new encoder_alphabetic_t{*this}; }

    character_t encoder_alphabetic_t::operator()(char arg) const
    {
        const invalid_code_t error{*this, arg};

        if (arg < 'a' or arg >= 'a' + alphabet().n_characters())
            throw error;

        return arg - 'a';
    }

    char encoder_alphabetic_t::operator()(character_t arg) const
    {
        const invalid_code_t error{*this, arg};

        if (arg >= alphabet().n_characters())
            throw error;

        return static_cast<char>('a' + arg);
    }

    invalid_code_t::invalid_code_t(const invalid_code_t &arg) :
        encoder_{arg.encoder_->clone()},
        code_{arg.code_} {}

    invalid_code_t &invalid_code_t::operator=(const invalid_code_t &arg)
        { return *this = invalid_code_t{arg}; }

    invalid_code_t::invalid_code_t(const encoder_t &encoder, char code) :
        encoder_{encoder.clone()}, code_{code} {}

    invalid_code_t::invalid_code_t(const encoder_t &encoder, character_t code) :
        encoder_{encoder.clone()}, code_{code} {}

    const encoder_t &invalid_code_t::encoder() const { return *encoder_; }
    const std::variant<char, character_t> &invalid_code_t::code() const { return code_; }
}

