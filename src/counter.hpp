#pragma once
#include "machine.hpp"
#include "number.hpp"

namespace Machine
{
    class counter_t;
    class counter_operation_t;
    class counter_operation_inc_t;
    class counter_operation_dec_t;
    class counter_operation_zero_t;
    class counter_operation_pos_t;
    class counter_operation_neg_t;
    class counter_initialiser_t;
    class counter_initialiser_zero_t;
    class counter_initialiser_string_t;
    class counter_terminator_t;
    class counter_terminator_zero_t;
    class counter_terminator_string_t;

    class counter_initialiser_t : public initialiser_t
    {
    public:
        virtual ~counter_initialiser_t() = default;
        counter_initialiser_t *clone() const override = 0;
    };

    class counter_initialiser_zero_t final : public counter_initialiser_t
    {
    public:
        counter_initialiser_zero_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class counter_initialiser_string_t final : public counter_initialiser_t
    {
    public:
        counter_initialiser_string_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class counter_terminator_t : public terminator_t
    {
    public:
        virtual ~counter_terminator_t() = default;
        counter_terminator_t *clone() const override = 0;
    };

    class counter_terminator_zero_t final : public counter_terminator_t
    {
    public:
        counter_terminator_zero_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class counter_terminator_string_t final : public counter_terminator_t
    {
    public:
        counter_terminator_string_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class counter_t final : public device_t
    {
    private:
        std::unique_ptr<counter_initialiser_t> initialiser_;
        std::unique_ptr<counter_terminator_t> terminator_;
        signed_number_t state_{};

    public:
        counter_t() = delete;
        ~counter_t() = default;

        counter_t(const counter_t &);
        counter_t &operator=(const counter_t &);

        counter_t(counter_t &&) noexcept = default;
        counter_t &operator=(counter_t &&) noexcept = default;

        counter_t(std::unique_ptr<encoder_t>,
            std::unique_ptr<counter_initialiser_t>,
            std::unique_ptr<counter_terminator_t>);

        counter_t *clone() const override;

    public:
        const counter_initialiser_t &initialiser() const override;
        const counter_terminator_t &terminator() const override;
        std::string print_name() const override;
        std::string print_state() const override;

        signed_number_t &state();
        const signed_number_t &state() const;
    };

    class counter_operation_t : public operation_t
    {
    public:
        virtual ~counter_operation_t() = default;

    public:
        bool correct_device(const device_t &) const override;
    };

    class counter_operation_inc_t : public counter_operation_t
    {
    public:
        counter_operation_inc_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class counter_operation_dec_t : public counter_operation_t
    {
    public:
        counter_operation_dec_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class counter_operation_zero_t : public counter_operation_t
    {
    public:
        counter_operation_zero_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class counter_operation_pos_t : public counter_operation_t
    {
    public:
        counter_operation_pos_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class counter_operation_neg_t : public counter_operation_t
    {
    public:
        counter_operation_neg_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };
}

