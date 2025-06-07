#include "input.hpp"
#include <cassert>

namespace Machine
{
    input_initialiser_t *input_initialiser_t::clone() const { return new input_initialiser_t{}; }

    void input_initialiser_t::initialise(device_t &device, const string_t &string) const
    {
        dynamic_cast<input_t &>(device).string() = string;
        return;
    }

    input_terminator_t *input_terminator_t::clone() const { return new input_terminator_t{}; }

    bool input_terminator_t::terminating(const device_t &device) const
    { return dynamic_cast<const input_t &>(device).string().empty(); }

    string_t input_terminator_t::terminate_impl(const device_t &device) const
        { return {dynamic_cast<const input_t &>(device).alphabet()}; }

    input_initialiser_t input_t::initialiser_{};
    input_terminator_t input_t::terminator_{};

    input_t::input_t(const alphabet_t &alphabet) : string_{alphabet} {}
    input_t::input_t(const string_t &string) : string_{string} {}
    input_t::input_t(const std::string &string) : string_{string} {}
    input_t *input_t::clone() const { return new input_t{*this}; }

    const input_initialiser_t &input_t::initialiser() const { return initialiser_; }
    const input_terminator_t &input_t::terminator() const { return terminator_; }

    string_t &input_t::string() { return string_; }
    const string_t &input_t::string() const { return string_; }
    const alphabet_t &input_t::alphabet() const { return string_.alphabet(); }

    input_operation_t::~input_operation_t() {}

    bool input_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(input_t); }

    bool input_operation_t::intersecting_domain(const terminator_t &) const
        { return false; }

    input_scan_operation_t::input_scan_operation_t(character_t character) :
        character_{character} {}

    input_scan_operation_t *input_scan_operation_t::clone() const
        { return new input_scan_operation_t{*this}; }

    bool input_scan_operation_t::applicable(const device_t &device) const
        { return dynamic_cast<const input_t &>(device).string().see() == character_; }

    bool input_scan_operation_t::intersecting_domain(const operation_t &arg) const
        { return character_ == dynamic_cast<const input_scan_operation_t &>(arg).character_; }

    void input_scan_operation_t::apply_impl(device_t &device) const
    {
        dynamic_cast<input_t &>(device).string().pop();
        return;
    }
}

