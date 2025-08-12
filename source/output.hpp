#pragma once
#include "machine.hpp"

namespace Machine
{
    class output_t;
    class output_operation_write_t;
    class output_initialiser_t;
    class output_terminator_t;

    class output_initialiser_t final : public initialiser_t
    {
    public:
        output_initialiser_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class output_terminator_t final : public terminator_t
    {
    public:
        output_terminator_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class output_t final : public device_t
    {
    private:
        static output_initialiser_t initialiser_;
        static output_terminator_t terminator_;

        string_t string_{encoder().alphabet()};

    public:
        output_t() = delete;
        ~output_t() = default;

        output_t(const output_t &);
        output_t &operator=(const output_t &);

        output_t(output_t &&) noexcept = default;
        output_t &operator=(output_t &&) noexcept = default;

        output_t(std::unique_ptr<encoder_t>);
        output_t *clone() const override;

    public:
        const output_initialiser_t &initialiser() const override;
        const output_terminator_t &terminator() const override;
        std::string print_state() const override;

        string_t &string();
        const string_t &string() const;
    };

    class output_operation_write_t final : public operation_t
    {
    private:
        character_t character_;

    public:
        output_operation_write_t(character_t);
        output_operation_write_t *clone() const override;

    public:
        bool correct_device(const device_t &) const override;
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };
}

