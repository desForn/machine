#pragma once
#include "machine.hpp"

#include <unordered_map>
#include <ranges>

namespace Machine
{
    class control_t;
    class control_operation_t;
    class control_initialiser_t;
    class control_terminator_t;

    class control_initialiser_t final : public initialiser_t
    {
    private:
        index_t initial_state_;

    public:
        control_initialiser_t(index_t) noexcept;
        control_initialiser_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
        index_t initial_state() const;
    };

    class control_terminator_t final : public terminator_t
    {
    private:
        std::unordered_map<index_t, string_t> terminating_states_;

    public:
        control_terminator_t(std::unordered_map<index_t, string_t>);
        control_terminator_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
        bool terminating_state(index_t) const;
        const std::unordered_map<index_t, string_t> &terminating_states() const;
    };

    class control_t final : public device_t
    {
    private:
        index_t state_{negative_1};
        std::unique_ptr<control_initialiser_t> initialiser_;
        std::unique_ptr<control_terminator_t> terminator_;

    public:
        control_t() = delete;
        ~control_t() = default;

        control_t(const control_t &);
        control_t &operator=(const control_t &);

        control_t(control_t &&) noexcept = default;
        control_t &operator=(control_t &&) noexcept = default;

        control_t(std::unique_ptr<encoder_t>, std::unique_ptr<control_initialiser_t>,
                std::unique_ptr<control_terminator_t>);
        control_t *clone() const override;

    public:
        const control_initialiser_t &initialiser() const override;
        const control_terminator_t &terminator() const override;
        std::string print_name() const override;
        std::string print_state() const override;

        index_t &state() noexcept;
        const index_t &state() const noexcept;
    };

    class control_operation_t final : public operation_t
    {
    private:
        index_t from_;
        index_t to_;

    public:
        control_operation_t(index_t, index_t) noexcept;
        control_operation_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool correct_device(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

    public:  
        index_t from() const noexcept;
        index_t to() const noexcept;
    };
}

