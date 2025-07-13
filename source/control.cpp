#include "control.hpp"

namespace Machine
{
    control_initialiser_t::control_initialiser_t() : initial_state_{0} {}

    control_initialiser_t::control_initialiser_t(index_t initial_state) :
        initial_state_{initial_state} {}

    control_initialiser_t *control_initialiser_t::clone() const
        { return new control_initialiser_t{*this}; }

    void control_initialiser_t::initialise(device_t &device, const string_t &) const
        { dynamic_cast<control_t &>(device).state() = initial_state_; }

    index_t control_initialiser_t::initial_state() const { return initial_state_; }

    control_terminator_t::control_terminator_t
        (std::unordered_map<index_t, string_t> terminating_states) :
            terminating_states_{std::move(terminating_states)}
    {
        const alphabet_t *a = nullptr;
        for (const auto &i : terminating_states_)
        {
            if (not a)
                a = &i.second.alphabet();
            else
                if (i.second.alphabet() != *a)
                    throw std::runtime_error{
           "In control_terminator_t::control_terminator_t(std::unordered_map<index_t, string_t>)."};
        }

        return;
    }

    control_terminator_t *control_terminator_t::clone() const
        { return new control_terminator_t{*this}; }

    bool control_terminator_t::terminating(const device_t &device) const
        { return terminating_state(dynamic_cast<const control_t &>(device).state()); }

    string_t control_terminator_t::terminate(const device_t &device) const
    {
        auto it = terminating_states_.find(dynamic_cast<const control_t &>(device).state());

        if (it == std::end(terminating_states_))
            throw invalid_terminator_t(*this, device);

        return it->second;
    }

    bool control_terminator_t::terminating_state(index_t state) const
        { return terminating_states_.contains(state); }

    const std::unordered_map<index_t, string_t> &control_terminator_t::terminating_states() const
        { return terminating_states_; }

    control_t::control_t(control_terminator_t terminator) :
        initialiser_{}, terminator_{std::move(terminator)} {}

    control_t::control_t(control_initialiser_t initialiser, control_terminator_t terminator):
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    control_t::control_t(std::unordered_map<index_t, string_t> terminator) :
        initialiser_{}, terminator_{std::move(terminator)} {}

    control_t::control_t(control_initialiser_t initialiser,
            std::unordered_map<index_t, string_t> terminator):
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    control_t *control_t::clone() const
        { return new control_t{*this}; }

    const control_initialiser_t &control_t::initialiser() const { return initialiser_; }

    const control_terminator_t &control_t::terminator() const { return terminator_; }

    index_t &control_t::state() { return state_; }

    const index_t &control_t::state() const { return state_; }

    control_operation_t::control_operation_t(index_t from, index_t to) : from_{from}, to_{to} {}

    control_operation_t *control_operation_t::clone() const
        { return new control_operation_t{*this}; }

    bool control_operation_t::applicable(const device_t &device) const
        { return dynamic_cast<const control_t &>(device).state() == from_; }

    void control_operation_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        dynamic_cast<control_t &>(device).state() = to_;
        return;
    }

    bool control_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(control_t); }

    bool control_operation_t::intersecting_domain(const operation_t &arg) const
        { return from_ == dynamic_cast<const control_operation_t &>(arg).from_; }

    bool control_operation_t::intersecting_domain(const terminator_t &arg) const
        { return dynamic_cast<const control_terminator_t &>(arg).terminating_state(from_); }

    index_t control_operation_t::from() const { return from_; }

    index_t control_operation_t::to() const { return to_; }
}

