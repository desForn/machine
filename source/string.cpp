#include "string.hpp"

#include <algorithm>

namespace Machine
{
    alphabet_t::alphabet_t(character_t n_characters) : n_characters_{n_characters} {}
    character_t alphabet_t::n_characters() const { return n_characters_; }

    string_t::string_t() {}
    string_t::string_t(const alphabet_t &alphabet) : alphabet_{alphabet} {}
    string_t::string_t(const alphabet_t &alphabet, std::vector<character_t> string) :
        alphabet_{alphabet}, string_{std::move(string)}
    {
        if (std::ranges::any_of(string_, [n = alphabet_.n_characters()](const character_t &c)
                    { return c >= n; }))
            throw std::runtime_error
                {"In string_t::string_t(const alphabet_t &, std::vector<character_t>)."};
        return;
    }

    string_t::string_t(const std::string &string) : alphabet_{256}
    {
        string_.resize(std::size(string));

        auto j = std::rbegin(string_);
        for (auto i = std::cbegin(string); i != std::cend(string); ++i, ++j)
            *j = static_cast<character_t>(*i);
        pos_ = std::size(string) - 1;
        return;
    }

    bool string_t::empty() const { return pos_ == negative_1; }

    character_t string_t::see() const { return string_[pos_]; }

    character_t string_t::pop()
    {
        if (pos_ == negative_1)
            throw read_past_eof_t{};
        return string_[pos_--];
    }

    void string_t::push(character_t c)
    {
        if (c >= alphabet_.n_characters())
            throw invalid_character_t{};
        ++pos_;
        if (pos_ == std::size(string_))
            string_.push_back(c);
        else
            string_[pos_] = c;
        return;
    }

    index_t string_t::get_pos()
        { return pos_; }

    void string_t::set_pos(index_t pos)
    {
        if (pos >= std::size(string_))
            throw string_overflow_t{};
        pos_ = pos;
        return;
    }

    string_t &string_t::clear() { string_.clear(); pos_ = negative_1; return *this; }

    const alphabet_t &string_t::alphabet() const { return alphabet_; }

    std::string string_t::to_ascii() const
    {
        if (alphabet().n_characters() != 256)
            throw std::runtime_error{"In string_t::to_ascii()."};

        std::string ret;
        for (index_t i = pos_; i != negative_1; --i)
            ret.push_back(static_cast<char>(string_[i]));

        return ret;
    }
}

