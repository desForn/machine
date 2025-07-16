#include "encoder.hpp"

namespace Machine
{
    encoder_t::encoder_t(const alphabet_t &alphabet) : alphabet_{alphabet} {}

    const alphabet_t &encoder_t::alphabet() const { return alphabet_; }

    encoder_alphabetical_t::encoder_alphabetical_t(character_t n) : encoder_t{alphabet_t{n}}
    {
        if (n > 26)
            throw std::runtime_error{"In encoder_alphabetical_t::encoder_alphabetical_t(index_t).\n"
                "Maximum number of characters in the alphabet is 26."};
        return;
    }

    encoder_alphabetical_t::encoder_alphabetical_t(const alphabet_t alphabet) : encoder_t{alphabet}
    {
        if (alphabet_.n_characters() > 26)
            throw std::runtime_error{"In encoder_alphabetical_t::encoder_alphabetical_t"
                "(const alphabet_t &).\n"
                "Maximum number of characters in the alphabet is 26."};
        return;
    }

    encoder_alphabetical_t *encoder_alphabetical_t::clone() const
        { return new encoder_alphabetical_t{*this}; }

    character_t encoder_alphabetical_t::operator()(char arg) const
    {
        const invalid_code_t error{*this, arg};

        if (arg < 'a' or arg >= 'a' + alphabet().n_characters())
            throw error;

        return arg - 'a';
    }

    char encoder_alphabetical_t::operator()(character_t arg) const
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

