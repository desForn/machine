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
        void initialise(device_t &, const string_t &) const override;
    };

    class output_terminator_t final : public output_t
    {
    public:
        output_terminator_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        string_t terminate(const device_t &) const override;
    };

    class output_t final : public device_t
    {
    private:
        static output_initialiser_t initialiser_;
        static output_terminator_t terminator_;

        string_t string_;

    public:
        output_t(const alphabet_t &);
        output_t *clone() const override;

    public:
        const output_initialiser_t &initialiser() const override;
        const output_terminator_t &terminator() const override;

        string_t &string();
        const string_t &string() const;
        const alphabet_t &alphabet() const;
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

