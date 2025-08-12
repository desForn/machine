#include "counter.hpp"
#include <cstdlib>
#include <limits>

namespace Machine
{
    counter_initialiser_zero_t *counter_initialiser_zero_t::clone() const
        { return new counter_initialiser_zero_t{*this}; }

    void counter_initialiser_zero_t::initialise(device_t &device, const std::string &) const
        { dynamic_cast<counter_t &>(device).state() = 0; }

    counter_initialiser_string_t *counter_initialiser_string_t::clone() const
        { return new counter_initialiser_string_t{*this}; }

    void counter_initialiser_string_t::initialise
        (device_t &device, const std::string &string) const
    {
        dynamic_cast<counter_t &>(device).state() =
            signed_number_t{string, device.encoder()};
        return;
    }

    counter_terminator_zero_t *counter_terminator_zero_t::clone() const
        { return new counter_terminator_zero_t{*this}; }

    bool counter_terminator_zero_t::terminating(const device_t &device) const
        { return dynamic_cast<const counter_t &>(device).state() == 0; }

    std::string counter_terminator_zero_t::terminate(const device_t &) const { return {}; }

    counter_terminator_string_t *counter_terminator_string_t::clone() const
        { return new counter_terminator_string_t{*this}; }

    bool counter_terminator_string_t::terminating(const device_t &) const { return true; }

    std::string counter_terminator_string_t::terminate(const device_t &device) const
    {
        return dynamic_cast<const counter_t &>(device).state().print(device.encoder());
    }

    counter_t::counter_t(const counter_t &arg) :
        device_t{std::unique_ptr<encoder_t>{arg.encoder().clone()}},
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()},
        state_{arg.state_} {}

    counter_t &counter_t::operator=(const counter_t &arg) { return *this = counter_t{arg}; }

    counter_t::counter_t
        (std::unique_ptr<encoder_t> encoder,
         std::unique_ptr<counter_initialiser_t> initialiser,
         std::unique_ptr<counter_terminator_t> terminator) :
        device_t{std::move(encoder)},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    counter_t *counter_t::clone() const
        { return new counter_t{*this}; }

    const counter_initialiser_t &counter_t::initialiser() const
        { return *initialiser_; }

    const counter_terminator_t &counter_t::terminator() const
        { return *terminator_; }

    std::string counter_t::print_state() const { return state_.print(encoder()); }

    signed_number_t &counter_t::state() { return state_; }

    const signed_number_t &counter_t::state() const { return state_; }

    bool counter_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(counter_t); }

    counter_operation_inc_t *counter_operation_inc_t::clone() const
        { return new counter_operation_inc_t{*this}; }

    bool counter_operation_inc_t::applicable(const device_t &) const { return true; }

    void counter_operation_inc_t::apply(device_t &device) const
        { ++(dynamic_cast<counter_t &>(device).state()); }

    bool counter_operation_inc_t::intersecting_domain(const operation_t &) const
        { return true; }

    bool counter_operation_inc_t::intersecting_domain(const terminator_t &) const
        { return true; }

    counter_operation_dec_t *counter_operation_dec_t::clone() const
        { return new counter_operation_dec_t{*this}; }

    bool counter_operation_dec_t::applicable(const device_t &) const { return true; }

    void counter_operation_dec_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        --(dynamic_cast<counter_t &>(device).state());

        return;
    }

    bool counter_operation_dec_t::intersecting_domain(const operation_t &operation) const
        { return typeid(operation) != typeid(counter_operation_zero_t); }

    bool counter_operation_dec_t::intersecting_domain(const terminator_t & terminator) const 
        { return typeid(terminator) != typeid(counter_terminator_zero_t); }

    counter_operation_zero_t *counter_operation_zero_t::clone() const
        { return new counter_operation_zero_t{*this}; }

    bool counter_operation_zero_t::applicable(const device_t &device) const
        { return dynamic_cast<const counter_t &>(device).state().zero(); }

    void counter_operation_zero_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        return;
    }

    bool counter_operation_zero_t::intersecting_domain(const operation_t &operation) const
    {
        return typeid(operation) != typeid(counter_operation_pos_t) and
            typeid(operation) != typeid(counter_operation_neg_t);
    }

    bool counter_operation_zero_t::intersecting_domain (const terminator_t &) const { return true; }

    counter_operation_pos_t *counter_operation_pos_t::clone() const
        { return new counter_operation_pos_t{*this}; }

    bool counter_operation_pos_t::applicable(const device_t &device) const
        { return dynamic_cast<const counter_t &>(device).state().pos(); }

    void counter_operation_pos_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        return;
    }

    bool counter_operation_pos_t::intersecting_domain
        (const operation_t &operation) const
    {
        return typeid(operation) != typeid(counter_operation_zero_t) and
            typeid(operation) != typeid(counter_operation_neg_t);
    }

    bool counter_operation_pos_t::intersecting_domain
        (const terminator_t &terminator) const
        { return typeid(terminator) == typeid(counter_terminator_string_t); }

    counter_operation_neg_t *counter_operation_neg_t::clone() const
        { return new counter_operation_neg_t{*this}; }

    bool counter_operation_neg_t::applicable(const device_t &device) const
        { return dynamic_cast<const counter_t &>(device).state().neg(); }

    void counter_operation_neg_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        return;
    }

    bool counter_operation_neg_t::intersecting_domain
        (const operation_t &operation) const
    {
        return typeid(operation) != typeid(counter_operation_zero_t) and
            typeid(operation) != typeid(counter_operation_pos_t);
    }

    bool counter_operation_neg_t::intersecting_domain
        (const terminator_t &terminator) const
        { return typeid(terminator) == typeid(counter_terminator_string_t); }
}

