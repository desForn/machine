#pragma once
#include "machine.hpp"
#include "number.hpp"

namespace Machine
{
    class unsigned_counter_t;
    class unsigned_counter_operation_t;
    class unsigned_counter_operation_inc_t;
    class unsigned_counter_operation_dec_t;
    class unsigned_counter_operation_zero_t;
    class unsigned_counter_operation_non_zero_t;
    class unsigned_counter_initialiser_t;
    class unsigned_counter_initialiser_zero_t;
    class unsigned_counter_initialiser_string_t;
    class unsigned_counter_terminator_t;
    class unsigned_counter_terminator_zero_t;
    class unsigned_counter_terminator_string_t;

    class unsigned_counter_initialiser_t : public initialiser_t
    {
    public:
        virtual ~unsigned_counter_initialiser_t() = default;
        unsigned_counter_initialiser_t *clone() const override = 0;
    };

    class unsigned_counter_initialiser_zero_t final : public unsigned_counter_initialiser_t
    {
    public:
        unsigned_counter_initialiser_zero_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class unsigned_counter_initialiser_string_t final : public unsigned_counter_initialiser_t
    {
    public:
        unsigned_counter_initialiser_string_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class unsigned_counter_terminator_t : public terminator_t
    {
    public:
        virtual ~unsigned_counter_terminator_t() = default;
        unsigned_counter_terminator_t *clone() const override = 0;
    };

    class unsigned_counter_terminator_zero_t final : public unsigned_counter_terminator_t
    {
    public:
        unsigned_counter_terminator_zero_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class unsigned_counter_terminator_string_t final : public unsigned_counter_terminator_t
    {
    public:
        unsigned_counter_terminator_string_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class unsigned_counter_t final : public device_t
    {
    private:
        std::unique_ptr<unsigned_counter_initialiser_t> initialiser_;
        std::unique_ptr<unsigned_counter_terminator_t> terminator_;
        unsigned_number_t state_{};

    public:
        unsigned_counter_t() = delete;
        ~unsigned_counter_t() = default;

        unsigned_counter_t(const unsigned_counter_t &);
        unsigned_counter_t &operator=(const unsigned_counter_t &);

        unsigned_counter_t(unsigned_counter_t &&) noexcept = default;
        unsigned_counter_t &operator=(unsigned_counter_t &&) noexcept = default;

        unsigned_counter_t(std::unique_ptr<encoder_t>,
               std::unique_ptr<unsigned_counter_initialiser_t>,
               std::unique_ptr<unsigned_counter_terminator_t>);

        unsigned_counter_t *clone() const override;

    public:
        const unsigned_counter_initialiser_t &initialiser() const override;
        const unsigned_counter_terminator_t &terminator() const override;
        std::string print_name() const override;
        std::string print_state() const override;

        unsigned_number_t &state();
        const unsigned_number_t &state() const;
    };

    class unsigned_counter_operation_t : public operation_t
    {
    public:
        virtual ~unsigned_counter_operation_t() = default;

    public:
        bool correct_device(const device_t &) const override;
    };

    class unsigned_counter_operation_inc_t : public unsigned_counter_operation_t
    {
    public:
        unsigned_counter_operation_inc_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class unsigned_counter_operation_dec_t : public unsigned_counter_operation_t
    {
    public:
        unsigned_counter_operation_dec_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class unsigned_counter_operation_zero_t : public unsigned_counter_operation_t
    {
    public:
        unsigned_counter_operation_zero_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class unsigned_counter_operation_non_zero_t : public unsigned_counter_operation_t
    {
    public:
        unsigned_counter_operation_non_zero_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };
}

