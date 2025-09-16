#pragma once

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
    class encoder_b_ary_t;
    class encoder_b_adic_t;
    class encoder_signed_b_ary_t;
    class encoder_signed_b_adic_t;
    class encoder_separator_t;
    using encoder_numeric_t = encoder_signed_b_ary_t;

    class encoder_t
    {
    protected:
        alphabet_t alphabet_;

    public:
        encoder_t(const alphabet_t &) noexcept;
        virtual ~encoder_t() = default;
        virtual encoder_t *clone() const = 0;

    public:
        const alphabet_t &alphabet() const noexcept;

        virtual character_t operator()(char c) const = 0;
        virtual char operator()(character_t c) const = 0;
        string_t operator()(const std::string &) const;
        std::string operator()(const string_t &) const;
    };

    class encoder_ascii_t final : public encoder_t
    {
    public:
        encoder_ascii_t() noexcept;
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
        encoder_alphabetic_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;
    };

    class encoder_b_ary_t final : public encoder_t
    {
    public:
        encoder_b_ary_t(character_t);
        encoder_b_ary_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;
    };

    class encoder_b_adic_t final : public encoder_t
    {
    public:
        encoder_b_adic_t(character_t);
        encoder_b_adic_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;
    };

    class encoder_signed_b_ary_t final : public encoder_t
    {
    public:
        encoder_signed_b_ary_t(character_t);
        encoder_signed_b_ary_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;
    };

    class encoder_signed_b_adic_t final : public encoder_t
    {
    public:
        encoder_signed_b_adic_t(character_t);
        encoder_signed_b_adic_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;
    };

    class encoder_separator_t final : public encoder_t
    {
    private:
        std::unique_ptr<encoder_t> extended_encoder_;
        char separator_;

    public:
        encoder_separator_t() = delete;
        ~encoder_separator_t() = default;

        encoder_separator_t(const encoder_separator_t &);
        encoder_separator_t &operator=(const encoder_separator_t &);

        encoder_separator_t(encoder_separator_t &&) noexcept = default;
        encoder_separator_t &operator=(encoder_separator_t &&) noexcept = default;

        encoder_separator_t(std::unique_ptr<encoder_t>, char);
        encoder_separator_t *clone() const override;

    public:
        character_t operator()(char) const override;
        char operator()(character_t) const override;
        using encoder_t::operator();
        using encoder_t::alphabet;

        decltype(auto) extended_encoder(this auto &&self) { return (*(self.extended_encoder_)); }
        char separator() const;
    };
}

