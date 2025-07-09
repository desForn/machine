#pragma once
#include "fwd.hpp"

#include <string>
#include <vector>
#include <stdexcept>

namespace Machine
{
    class alphabet_t;
    class ascii_alphabet_t;
    class string_t;

    class read_past_eof_t : public std::exception {};
    class invalid_character_t : public std::exception {};
    class string_overflow_t : public std::exception {};

    class alphabet_t
    {
    private:
        character_t n_characters_;

    public:
        virtual ~alphabet_t() = default;

    public:
        alphabet_t(character_t n_characters);
        character_t n_characters() const;
    };

    class string_t
    {
    private:
        alphabet_t alphabet_{0};
        std::vector<character_t> string_{};
        index_t pos_{negative_1};

    public:
        string_t();
        string_t(const alphabet_t &);
        string_t(const alphabet_t &, std::vector<character_t>);
        string_t(const std::string &);

    public:
        bool empty() const;
        character_t see() const;
        character_t pop();
        void push(character_t);

        index_t get_pos();
        void set_pos(index_t);

        string_t &clear();

        const alphabet_t &alphabet() const;
        std::string to_ascii() const;
    };
}

