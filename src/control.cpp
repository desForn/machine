#include "control.hpp"

namespace Machine
{
    control_initialiser_t::control_initialiser_t(index_t initial_state) noexcept :
        initial_state_{initial_state} {}

    control_initialiser_t *control_initialiser_t::clone() const
        { return new control_initialiser_t{*this}; }

    void control_initialiser_t::initialise(device_t &device, const std::string &) const
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
                    throw std::runtime_error{"In Machine::control_terminator_t::"
                        "control_terminator_t(std::unordered_map<index_t, string_t>):\n"
                        "Inconsistent alphabets of the strings.\n"};
        }

        return;
    }

    control_terminator_t *control_terminator_t::clone() const
        { return new control_terminator_t{*this}; }

    bool control_terminator_t::terminating(const device_t &device) const
        { return terminating_state(dynamic_cast<const control_t &>(device).state()); }

    std::string control_terminator_t::terminate(const device_t &device) const
    {
        auto it = terminating_states_.find(dynamic_cast<const control_t &>(device).state());

        if (it == std::end(terminating_states_))
            throw invalid_terminator_t(*this, device);

        return device.encoder()(it->second);
    }

    bool control_terminator_t::terminating_state(index_t state) const
        { return terminating_states_.contains(state); }

    const std::unordered_map<index_t, string_t> &control_terminator_t::terminating_states() const
        { return terminating_states_; }

    control_t::control_t(std::unique_ptr<encoder_t> encoder,
            std::unique_ptr<control_initialiser_t> initialiser,
            std::unique_ptr<control_terminator_t> terminator):
        device_t{std::move(encoder)},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)}
    {
        if (not encoder_)
            encoder_.reset(new encoder_ascii_t{});
    }

    control_t::control_t(const control_t &arg) :
        device_t{std::unique_ptr<encoder_t>(arg.encoder_->clone())},
        state_{arg.state_},
        initialiser_{std::unique_ptr<control_initialiser_t>{arg.initialiser_->clone()}},
        terminator_{std::unique_ptr<control_terminator_t>{arg.terminator_->clone()}} {}

    control_t &control_t::operator=(const control_t &arg) { return *this = control_t{arg}; }

    control_t *control_t::clone() const { return new control_t{*this}; }

    const control_initialiser_t &control_t::initialiser() const { return *initialiser_; }

    const control_terminator_t &control_t::terminator() const { return *terminator_; }

    std::string control_t::print_name() const { return "Control"; }

    std::string control_t::print_state() const { return std::to_string(state_); }

    index_t &control_t::state() noexcept { return state_; }

    const index_t &control_t::state() const noexcept { return state_; }

    control_operation_t::control_operation_t(index_t from, index_t to) noexcept :
        from_{from}, to_{to} {}

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

    bool control_operation_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(control_operation_t)) 
            return from_ == dynamic_cast<const control_operation_t &>(operation).from_;
        if (typeid(operation) == typeid(noop_operation_t))
            return true;

        throw std::runtime_error{
            "In Machine::control_operation_t::intersecting_domain(const operation_t &):\n"
            "Unknown operation.\n"};
    }

    bool control_operation_t::intersecting_domain(const terminator_t &arg) const
        { return dynamic_cast<const control_terminator_t &>(arg).terminating_state(from_); }

    index_t control_operation_t::from() const noexcept { return from_; }

    index_t control_operation_t::to() const noexcept { return to_; }
}

