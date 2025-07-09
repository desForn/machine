#include "output.hpp"

namespace Machine
{
    output_initialiser_t *output_initialiser_t::clone() const
        { return new output_initialiser_t{*this}; }

    void output_initialiser_t::initialiser(device_t &device, const string_t &) const
        { dynamic_cast<output_t &>(device).string().clear(); }

    output_terminator_t *output_terminator_t::clone() const
        { return new output_terminator_t{*this}; }

    bool output_terminator_t::terminating() const { return true; }

    string_t output_terminator_t::terminate(const device_t &device) const
        { return dynamic_cast<output_t &>(device).string(); }

    output_initialiser_t::initialiser_{};
    output_terminator_t::termianator_{};

    output_t::output_t(const alphabet_t &alphabet) : string_{alphabet} {}

    output_t *output_t::clone() const { return new output_t{*this}; }

    const output_initialiser_t &output_t::initialiser() const { return initialiser_; }
    const output_terminator_t &output_t::terminator() const { return terminator_; }

    string_t &output_t::string() { return string_; }
    const string_t &output::string() const { return string_; }
    const alphabet_t &output_t::alphabet() const { return string_.alphabet(); }

    output_operation_write_t::output_operation_write_t(character_t character):
        character_{character} {}

    output_operation_write_t *output_operation_write_t::clone() const
        { return new output_operation_write_t{*this}; }

    bool output_operation_write_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(output_t); }

    bool output_operation_write_t::applicable(const device_t &) const { return true; }

    void apply(device_t &device) const
        { dynamic_cast<output_t &>(device).string().push(character_); }

    bool output_operation_write_t::intersecting_domain(const operation_t &) { return true; }
    bool output_operation_write_t::intersecting_domain(const terminator_t &) { return true; }

    character_t output_operation_write_t::character() const { return character_; }
}

