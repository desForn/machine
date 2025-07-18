#pragma once
#include <string>
#include <memory>
#include <variant>
#include <unordered_map>

#include "string.hpp"

namespace Machine
{
    class encoder_t;
    class encoder_ascii_t;
    class encoder_list_t;
    class encoder_alphabetic_t;

    class invalid_code_t;

    class encoder_t
    {
    protected:
        alphabet_t alphabet_;

    public:
        encoder_t(const alphabet_t &);
        virtual ~encoder_t() = default;
        virtual encoder_t *clone() const = 0;

    public:
        virtual character_t operator()(char) const = 0;
        virtual char operator()(character_t) const = 0;
        string_t operator()(const std::string &) const;
        std::string operator()(const string_t &) const;
        const alphabet_t &alphabet() const;
    };

    class encoder_ascii_t final : public encoder_t
    {
    public:
        encoder_ascii_t();
        encoder_ascii_t *clone() const override;

    private:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;
    };

    class encoder_list_t final : public encoder_t
    {
    private:
        std::vector<char> map_;
        std::unordered_map<char, character_t> inverse_map_;

    public:
        encoder_list_t(std::vector<char>);
        encoder_list_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;

        const std::vector<char> &list() const;
        const std::unordered_map<char, character_t> &inverse_map() const;
    };

    class encoder_alphabetic_t final : public encoder_t
    {
    public:
        encoder_alphabetic_t(character_t);
        encoder_alphabetic_t(const alphabet_t &alphabet_);
        encoder_alphabetic_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;
    };

    class invalid_code_t : public std::exception
    {
    private:
        std::unique_ptr<encoder_t> encoder_;
        std::variant<char, character_t> code_;

    public:
        invalid_code_t() = delete;
        ~invalid_code_t() = default;

        invalid_code_t(const invalid_code_t &);
        invalid_code_t &operator=(const invalid_code_t &);

        invalid_code_t(invalid_code_t &&) noexcept = default;
        invalid_code_t &operator=(invalid_code_t &&) noexcept = default;

        invalid_code_t(const encoder_t &, char);
        invalid_code_t(const encoder_t &, character_t);

    public:
        const encoder_t &encoder() const;
        const std::variant<char, character_t> &code() const;
    };
}

