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

    character_t string_t::see() const
    {
        if (pos_ == negative_1)
            throw read_past_eof_t{};
        return string_[pos_];
    }

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

    index_t string_t::get_pos() const { return pos_; }

    void string_t::set_pos(index_t pos)
    {
        if (not ((pos_ == negative_1 and std::empty(string_)) or pos_ < std::size(string_)))
            throw string_overflow_t{};
        pos_ = pos;
        return;
    }

    void string_t::move_l()
    {
        if (pos_ == negative_1)
            throw read_past_eof_t{};
        --pos_;
        return;
    }

    void string_t::move_r(character_t default_character)
    {
        if (pos_ == std::size(string_) - 1)
            string_.push_back(default_character);
        ++pos_;
        return;
    }

    void string_t::print(character_t character)
    {
        if (pos_ == negative_1)
            throw read_past_eof_t{};
        string_[pos_] = character;
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

    std::vector<character_t> &string_t::data() { return string_; }
    const std::vector<character_t> &string_t::data() const { return string_; }

    std::strong_ordering string_t::operator<=>(const string_t &arg) const
    {
        if (alphabet_ != arg.alphabet_)
            throw std::runtime_error{"In string_t::operator<=>(const string_t &) const."};


        for(auto i = std::crbegin(string_), j = std::crbegin(arg.string_); ;++i, ++j)
        {
            if (i == std::crend(string_))
            {
                if (j == std::crend(arg.string_))
                    return std::strong_ordering::equivalent;
                return std::strong_ordering::less;
            }
            if (j == std::crend(arg.string_))
                return std::strong_ordering::greater;

            if (*i < *j)
                return std::strong_ordering::less;
            if (*j < *i)
                return std::strong_ordering::greater;
        }
    }

    bool string_t::operator==(const string_t &arg) const
        { return (*this <=> arg) == std::strong_ordering::equivalent; }

    bool string_t::operator!=(const string_t &arg) const { return not (*this == arg); }
}

