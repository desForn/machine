#include "unsigned_counter.hpp"
#include <cstdlib>
#include <limits>

namespace Machine
{
    unsigned_counter_initialiser_zero_t *unsigned_counter_initialiser_zero_t::clone() const
        { return new unsigned_counter_initialiser_zero_t{*this}; }

    void unsigned_counter_initialiser_zero_t::initialise
        (device_t &device, const std::string &) const
        { dynamic_cast<unsigned_counter_t &>(device).state() = 0; }

    unsigned_counter_initialiser_string_t *unsigned_counter_initialiser_string_t::clone() const
        { return new unsigned_counter_initialiser_string_t{*this}; }

    void unsigned_counter_initialiser_string_t::initialise
        (device_t &device, const std::string &string) const
    {
        dynamic_cast<unsigned_counter_t &>(device).state() =
            unsigned_number_t{string, device.encoder()};

        return;
    }

    unsigned_counter_terminator_zero_t *unsigned_counter_terminator_zero_t::clone() const
        { return new unsigned_counter_terminator_zero_t{*this}; }

    bool unsigned_counter_terminator_zero_t::terminating(const device_t &device) const
        { return dynamic_cast<const unsigned_counter_t &>(device).state() == 0; }

    std::string unsigned_counter_terminator_zero_t::terminate(const device_t &) const { return {}; }

    unsigned_counter_terminator_string_t *unsigned_counter_terminator_string_t::clone() const
        { return new unsigned_counter_terminator_string_t{*this}; }

    bool unsigned_counter_terminator_string_t::terminating(const device_t &) const { return true; }

    std::string unsigned_counter_terminator_string_t::terminate(const device_t &device) const
    {
        return dynamic_cast<const unsigned_counter_t &>(device).state().print(device.encoder());
    }

    unsigned_counter_t::unsigned_counter_t(const unsigned_counter_t &arg) :
        device_t{std::unique_ptr<encoder_t>(arg.encoder_->clone())},
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()},
        state_{arg.state_} {}

    unsigned_counter_t &unsigned_counter_t::operator=(const unsigned_counter_t &arg)
        { return *this = unsigned_counter_t{arg}; }

    unsigned_counter_t::unsigned_counter_t(std::unique_ptr<encoder_t> encoder,
           std::unique_ptr<unsigned_counter_initialiser_t> initialiser,
           std::unique_ptr<unsigned_counter_terminator_t> terminator) :
        device_t{std::move(encoder)},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    unsigned_counter_t *unsigned_counter_t::clone() const
        { return new unsigned_counter_t{*this}; }

    const unsigned_counter_initialiser_t &unsigned_counter_t::initialiser() const
        { return *initialiser_; }

    const unsigned_counter_terminator_t &unsigned_counter_t::terminator() const
        { return *terminator_; }

    std::string unsigned_counter_t::print_state() const { return state_.print(encoder()); }

    unsigned_number_t &unsigned_counter_t::state() { return state_; }

    const unsigned_number_t &unsigned_counter_t::state() const { return state_; }

    bool unsigned_counter_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(unsigned_counter_t); }

    unsigned_counter_operation_inc_t *unsigned_counter_operation_inc_t::clone() const
        { return new unsigned_counter_operation_inc_t{*this}; }

    bool unsigned_counter_operation_inc_t::applicable(const device_t &) const { return true; }

    void unsigned_counter_operation_inc_t::apply(device_t &device) const
        { ++(dynamic_cast<unsigned_counter_t &>(device).state()); }

    bool unsigned_counter_operation_inc_t::intersecting_domain(const operation_t &) const
        { return true; }

    bool unsigned_counter_operation_inc_t::intersecting_domain(const terminator_t &) const
        { return true; }

    unsigned_counter_operation_dec_t *unsigned_counter_operation_dec_t::clone() const
        { return new unsigned_counter_operation_dec_t{*this}; }

    bool unsigned_counter_operation_dec_t::applicable(const device_t &device) const
        { return not dynamic_cast<const unsigned_counter_t &>(device).state().zero(); }

    void unsigned_counter_operation_dec_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        --(dynamic_cast<unsigned_counter_t &>(device).state());

        return;
    }

    bool unsigned_counter_operation_dec_t::intersecting_domain(const operation_t &operation) const
        { return typeid(operation) != typeid(unsigned_counter_operation_zero_t); }

    bool unsigned_counter_operation_dec_t::intersecting_domain(const terminator_t &terminator) const
        { return typeid(terminator) != typeid(unsigned_counter_terminator_zero_t); }

    unsigned_counter_operation_zero_t *unsigned_counter_operation_zero_t::clone() const
        { return new unsigned_counter_operation_zero_t{*this}; }

    bool unsigned_counter_operation_zero_t::applicable(const device_t &device) const
        { return dynamic_cast<const unsigned_counter_t &>(device).state().zero(); }

    void unsigned_counter_operation_zero_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        return;
    }

    bool unsigned_counter_operation_zero_t::intersecting_domain(const operation_t &operation) const
    {
        return typeid(operation) == typeid(unsigned_counter_operation_inc_t) or
            typeid(operation) == typeid(unsigned_counter_operation_zero_t);
    }

    bool unsigned_counter_operation_zero_t::intersecting_domain
        (const terminator_t &) const { return true; }

    unsigned_counter_operation_non_zero_t *unsigned_counter_operation_non_zero_t::clone() const
        { return new unsigned_counter_operation_non_zero_t{*this}; }

    bool unsigned_counter_operation_non_zero_t::applicable(const device_t &device) const
        { return not dynamic_cast<const unsigned_counter_t &>(device).state().zero(); }

    void unsigned_counter_operation_non_zero_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        return;
    }

    bool unsigned_counter_operation_non_zero_t::intersecting_domain
        (const operation_t &operation) const
        { return typeid(operation) != typeid(unsigned_counter_operation_zero_t); }

    bool unsigned_counter_operation_non_zero_t::intersecting_domain
        (const terminator_t &terminator) const
        { return typeid(terminator) == typeid(unsigned_counter_terminator_string_t); }
}

