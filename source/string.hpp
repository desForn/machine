#pragma once
#include "fwd.hpp"

#include <string>
#include <vector>
#include <stdexcept>
#include <compare>

namespace Machine
{
    class alphabet_t;
    class ascii_alphabet_t;
    class string_t;

    class invalid_character_t : public std::exception {};
    class invalid_alphabet_t : public std::exception {};
    class string_overflow_t : public std::exception {};
    class read_past_eof_t : public std::exception {};

    class alphabet_t
    {
    private:
        character_t n_characters_{0};

    public:
        virtual ~alphabet_t() = default;

    public:
        alphabet_t() = default;
        alphabet_t(character_t n_characters);
        character_t n_characters() const;
        bool operator==(const alphabet_t &) const = default;
        bool operator!=(const alphabet_t &) const = default;
    };

    class string_t
    {
    private:
        alphabet_t alphabet_{0};
        std::vector<character_t> string_{};
        index_t pos_{std::size(string_) - 1};

    public:
        string_t();
        string_t(const alphabet_t &);
        string_t(const alphabet_t &, std::vector<character_t>);
        string_t(const std::string &);

    public:
        bool empty() const;
        character_t see() const;
        bool see(character_t) const;
        character_t pop();
        void push(character_t);

        index_t get_pos() const;
        void set_pos(index_t);
        void move_l();
        void move_r(character_t);
        void print(character_t);

        string_t &clear();

        const alphabet_t &alphabet() const;
        std::string to_ascii() const;

        std::vector<character_t> &data();
        const std::vector<character_t> &data() const;

        std::strong_ordering operator<=>(const string_t &) const;
        bool operator==(const string_t &) const;
        bool operator!=(const string_t &) const;
    };
}

