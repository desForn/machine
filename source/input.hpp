#pragma once
#include "machine.hpp"

namespace Machine
{
    class input_t;
    class input_operation_t;
    class input_operation_scan_t;
    class input_operation_next_t;
    class input_operation_eof_t;
    class input_initialiser_t;
    class input_terminator_t;

    class input_initialiser_t final : public initialiser_t
    {
    public:
        input_initialiser_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class input_terminator_t final : public terminator_t
    {
    public:
        input_terminator_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class input_t final : public device_t
    {
    private:
        static input_initialiser_t initialiser_;
        static input_terminator_t terminator_;

        string_t string_{encoder().alphabet()};

    public:
        input_t() = delete;
        ~input_t() = default;

        input_t(const input_t &);
        input_t &operator=(const input_t &);

        input_t(input_t &&) noexcept = default;
        input_t &operator=(input_t &&) noexcept = default;

        input_t(std::unique_ptr<encoder_t>);
        input_t *clone() const override;

    public:
        const input_initialiser_t &initialiser() const override;
        const input_terminator_t &terminator() const override;
        std::string print_state() const override;

        string_t &string(); 
        const string_t &string() const; 
    };

    class input_operation_t : public operation_t
    {
    public:
        virtual ~input_operation_t() = default;

    public:
        bool correct_device(const device_t &) const override;
    };

    class input_operation_scan_t final : public input_operation_t 
    {
    private:
        character_t character_;

    public:
        input_operation_scan_t(character_t);
        input_operation_scan_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class input_operation_next_t final : public input_operation_t 
    {
    private:
        character_t character_;

    public:
        input_operation_next_t(character_t);
        input_operation_next_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class input_operation_eof_t final : public input_operation_t 
    {
    public:
        input_operation_eof_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };
}

