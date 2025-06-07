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
    class control_terminator_recogniser_t;

    class control_initialiser_t final : public initialiser_t
    {
    private:
        index_t initial_state_;

    public:
        control_initialiser_t();
        control_initialiser_t(index_t);
        control_initialiser_t *clone() const override;

    public:
        void initialise(device_t &, const string_t &) const override;
    };

    class control_terminator_t : public terminator_t
    {
    public:
        virtual ~control_terminator_t() = default;
        virtual control_terminator_t *clone() const = 0;

    public:
        virtual bool terminating_state(index_t) const = 0;
    };

    class control_terminator_recogniser_t final : public control_terminator_t
    {
    private:
        std::unordered_map<index_t, bool> terminating_states_;

    public:
        control_terminator_recogniser_t(std::unordered_map<index_t, bool>);
        control_terminator_recogniser_t(const std::ranges::input_range auto &accepting_states,
                                        const std::ranges::input_range auto &rejecting_states)
      {
            for (index_t i : accepting_states)
            {
                auto it = terminating_states_.find(i);
                if (it != std::end(terminating_states_))
                    throw std::runtime_error{
                        "In control_treminator_recogniser_t::control_terminator_recogniser_t(const"
                        "std::ranges::input_range auto&, const std::ranges::input_range auto%"};
                terminating_states_.emplace(i, true);
            }
            for (index_t i : rejecting_states)
            {
                auto it = terminating_states_.find(i);
                if (it != std::end(terminating_states_))
                    throw std::runtime_error{
                        "In control_treminator_recogniser_t::control_terminator_recogniser_t(const"
                        "std::ranges::input_range auto&, const std::ranges::input_range auto%"};
                terminating_states_.emplace(i, false);
            }

            return;
        }      control_terminator_recogniser_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        bool terminating_state(index_t) const override;
    private:
        string_t terminate_impl(const device_t &) const override;
    };

    class control_t final : public device_t
    {
    private:
        index_t state_{negative_1};
        index_t n_states_;
        control_initialiser_t initialiser_;
        std::unique_ptr<control_terminator_t> terminator_;

    public:
        control_t() = delete;
        ~control_t() = default;

        control_t(const control_t &);
        control_t &operator=(const control_t &);

        control_t(control_t &&) noexcept = default;
        control_t &operator=(control_t &&) noexcept = default;

        control_t(index_t, const control_terminator_t &);
        control_t(index_t, control_initialiser_t, const control_terminator_t &);
        control_t(index_t, std::unique_ptr<control_terminator_t>);
        control_t(index_t, control_initialiser_t, std::unique_ptr<control_terminator_t>);
        control_t *clone() const override;

    public:
        const control_initialiser_t &initialiser() const override;
        const control_terminator_t &terminator() const override;

        index_t &state();
        const index_t &state() const;
        index_t n_states() const;
    };

    class control_operation_t final : public operation_t
    {
    private:
        index_t from_;
        index_t to_;

    public:
        control_operation_t(index_t, index_t);
        control_operation_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;

        bool correct_device(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    private:
        void apply_impl(device_t &) const override;

    public:  
        index_t from() const;
        index_t to() const;
    };
}

