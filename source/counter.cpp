#include "counter.hpp"
#include <cstdlib>
#include <limits>

namespace Machine
{
    counter_initialiser_zero_t *counter_initialiser_zero_t::clone() const
        { return new counter_initialiser_zero_t{*this}; }

    void counter_initialiser_zero_t::initialise(device_t &device, const string_t &) const
        { dynamic_cast<counter_t &>(device).state() = 0; }

    counter_initialiser_ascii_t *counter_initialiser_ascii_t::clone() const
        { return new counter_initialiser_ascii_t{*this}; }

    void counter_initialiser_ascii_t::initialise
        (device_t &device, const string_t &string) const
    {
        dynamic_cast<counter_t &>(device).state() =
            static_cast<integer_t>(std::atoll(string.to_ascii().c_str()));
        return;
    }

    counter_initialiser_b_ary_t *counter_initialiser_b_ary_t::clone() const
        { return new counter_initialiser_b_ary_t{*this}; }

    void counter_initialiser_b_ary_t::initialise
        (device_t &device, const string_t &string) const
    {
        const integer_t radix = string.alphabet().n_characters() - 1;
        integer_t n = 0;
        bool negative = false;

        auto it = string.data().cbegin();
        
        if (*it == radix)
        {
            negative = true;
            ++it;
        }

        if (it == string.data().cend())
            throw std::runtime_error(
                    "In counter_initialiser_b_ary_t::initialise(device_t &, const string_t &).");

        for (; it != string.data().cend(); ++it)
        {
            if (*it == radix)
                throw std::runtime_error(
                    "In counter_initialiser_b_ary_t::initialise(device_t &, const string_t &).");
            n = radix * n + *it;
        }

        if (negative)
            n = -n;

        dynamic_cast<counter_t &>(device).state() = n;

        return;
    }

    counter_initialiser_b_adic_t *counter_initialiser_b_adic_t::clone() const
        { return new counter_initialiser_b_adic_t{*this}; }

    void counter_initialiser_b_adic_t::initialise
        (device_t &device, const string_t &string) const
    {
        const integer_t radix = string.alphabet().n_characters() - 1;
        integer_t n = 0;
        bool negative = false;

        auto it = string.data().cbegin();
        
        if (*it == radix)
        {
            negative = true;
            ++it;
        }

        if (it == string.data().cend())
            throw std::runtime_error(
                    "In counter_initialiser_b_adic_t::initialise(device_t &, const string_t &).");

        for (; it != string.data().cend(); ++it)
        {
            if (*it == radix)
                throw std::runtime_error(
                    "In counter_initialiser_b_adic_t::initialise(device_t &, const string_t &).");
            n = radix * n + *it + 1;
        }

        if (negative)
            n = -n;

        dynamic_cast<counter_t &>(device).state() = n;

        return;
    }

    counter_terminator_zero_t *counter_terminator_zero_t::clone() const
        { return new counter_terminator_zero_t{*this}; }

    bool counter_terminator_zero_t::terminating(const device_t &device) const
        { return dynamic_cast<const counter_t &>(device).state() == 0; }

    string_t counter_terminator_zero_t::terminate(const device_t &) const { return {}; }

    counter_terminator_string_t *counter_terminator_string_t::clone() const
        { return new counter_terminator_string_t{*this}; }

    bool counter_terminator_string_t::terminating(const device_t &) const { return true; }

    string_t counter_terminator_string_t::terminate(const device_t &device) const
        { return {std::to_string(dynamic_cast<const counter_t &>(device).state())}; }

    counter_t::counter_t(const counter_t &arg) :
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()},
        state_{arg.state_} {}

    counter_t::counter_t(const counter_initialiser_t &initialiser,
            const counter_terminator_t &terminator) :
        initialiser_{initialiser.clone()},
        terminator_{terminator.clone()} {}

    counter_t::counter_t
        (std::unique_ptr<counter_initialiser_t> initialiser,
         std::unique_ptr<counter_terminator_t> terminator) :
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    counter_t *counter_t::clone() const
        { return new counter_t{*this}; }

    const counter_initialiser_t &counter_t::initialiser() const
        { return *initialiser_; }

    const counter_terminator_t &counter_t::terminator() const
        { return *terminator_; }

    integer_t &counter_t::state() { return state_; }

    const integer_t &counter_t::state() const { return state_; }

    bool counter_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(counter_t); }

    counter_operation_inc_t *counter_operation_inc_t::clone() const
        { return new counter_operation_inc_t{*this}; }

    bool counter_operation_inc_t::applicable(const device_t &) const { return true; }

    void counter_operation_inc_t::apply(device_t &device) const
    {
        constexpr integer_t max = std::numeric_limits<integer_t>::max();
        integer_t &a = dynamic_cast<counter_t &>(device).state();

        if (a == max)
            throw std::overflow_error{"In counter_operation_inc_t::apply(device_t &)."};
        ++a;

        return;
    }

    bool counter_operation_inc_t::intersecting_domain(const operation_t &) const
        { return true; }

    bool counter_operation_inc_t::intersecting_domain(const terminator_t &) const
        { return true; }

    counter_operation_dec_t *counter_operation_dec_t::clone() const
        { return new counter_operation_dec_t{*this}; }

    bool counter_operation_dec_t::applicable(const device_t &) const { return true; }

    void counter_operation_dec_t::apply(device_t &device) const
    {
        constexpr integer_t min = std::numeric_limits<integer_t>::min();
        integer_t &a = dynamic_cast<counter_t &>(device).state();

        if (a == min)
            throw std::overflow_error{"In counter_operation_dec_t::apply(device_t &)."};
        --a;

        return;
    }

    bool counter_operation_dec_t::intersecting_domain(const operation_t &) const { return true; }

    bool counter_operation_dec_t::intersecting_domain(const terminator_t &) const { return true; }

    counter_operation_zero_t *counter_operation_zero_t::clone() const
        { return new counter_operation_zero_t{*this}; }

    bool counter_operation_zero_t::applicable(const device_t &device) const
        { return dynamic_cast<const counter_t &>(device).state() == 0; }

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
        { return dynamic_cast<const counter_t &>(device).state() > 0; }

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
        { return dynamic_cast<const counter_t &>(device).state() < 0; }

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

