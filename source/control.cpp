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

    control_terminator_recogniser_t::control_terminator_recogniser_t
        (std::unordered_map<index_t, bool> terminating_states) :
            terminating_states_{std::move(terminating_states)} {}

    control_terminator_recogniser_t *control_terminator_recogniser_t::clone() const
        { return new control_terminator_recogniser_t{*this}; }

    bool control_terminator_recogniser_t::terminating(const device_t &device) const
        { return terminating_states_.contains(dynamic_cast<const control_t &>(device).state()); }

    bool control_terminator_recogniser_t::terminating_state(index_t state) const
        { return terminating_states_.contains(state); }

    string_t control_terminator_recogniser_t::terminate_impl(const device_t &device) const
    {
        index_t state = dynamic_cast<const control_t &>(device).state();
        if (terminating_states_.at(state) == false)
            return {"Reject"};
        return {"Accept"};
    }

    control_t::control_t(const control_t &arg) :
        state_{arg.state_}, n_states_{arg.n_states_}, initialiser_{arg.initialiser_},
        terminator_{arg.terminator_->clone()} {}

    control_t &control_t::operator=(const control_t &arg)
    {
        state_ = arg.state_;
        n_states_ = arg.n_states_;
        initialiser_ = arg.initialiser_;
        terminator_.reset(arg.terminator_->clone());
        return *this;
    }

    control_t::control_t(index_t n_states, const control_terminator_t &terminator) :
        n_states_{n_states}, initialiser_{}, terminator_{terminator.clone()} {}

    control_t::control_t(index_t n_states, control_initialiser_t initialiser,
            const control_terminator_t &terminator) :
        n_states_{n_states}, initialiser_{std::move(initialiser)},
        terminator_{terminator.clone()} {}

    control_t::control_t(index_t n_states, std::unique_ptr<control_terminator_t> terminator) :
        n_states_{n_states}, initialiser_{}, terminator_{std::move(terminator)} {}

    control_t::control_t(index_t n_states, control_initialiser_t initialiser,
            std::unique_ptr<control_terminator_t> terminator) :
        n_states_{n_states}, initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    control_t *control_t::clone() const
        { return new control_t{*this}; }

    const control_initialiser_t &control_t::initialiser() const { return initialiser_; }

    const control_terminator_t &control_t::terminator() const { return *terminator_; }

    index_t &control_t::state() { return state_; }

    const index_t &control_t::state() const { return state_; }

    index_t control_t::n_states() const { return n_states_; }

    control_operation_t::control_operation_t(index_t from, index_t to) : from_{from}, to_{to} {}

    control_operation_t *control_operation_t::clone() const
        { return new control_operation_t{*this}; }

    bool control_operation_t::applicable(const device_t &device) const
        { return dynamic_cast<const control_t &>(device).state() == from_; }

    bool control_operation_t::correct_device(const device_t &device) const
    {
        if (typeid(device) != typeid(control_t))
            return false;
        index_t n = dynamic_cast<const control_t &>(device).n_states();

        return from_ < n and to_ < n;
    }

    bool control_operation_t::intersecting_domain(const operation_t &arg) const
        { return from_ == dynamic_cast<const control_operation_t &>(arg).from_; }

    bool control_operation_t::intersecting_domain(const terminator_t &arg) const
        { return dynamic_cast<const control_terminator_t &>(arg).terminating_state(from_); }

    void control_operation_t::apply_impl(device_t &device) const
    {
        dynamic_cast<control_t &>(device).state() = to_;
        return;
    }

    index_t control_operation_t::from() const { return from_; }

    index_t control_operation_t::to() const { return to_; }
}

