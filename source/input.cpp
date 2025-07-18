#include "input.hpp"
#include <cassert>

namespace Machine
{
    input_initialiser_t *input_initialiser_t::clone() const { return new input_initialiser_t{}; }

    void input_initialiser_t::initialise(device_t &device, const string_t &string) const
    {
        string_t &string_ = dynamic_cast<input_t &>(device).string();
        if (string_.alphabet() != string.alphabet())
            throw std::runtime_error(
                    "In input_initialiser_t::initialise(device_t &, cons string_t &)");
        string_ = string;
        return;
    }

    input_terminator_t *input_terminator_t::clone() const { return new input_terminator_t{}; }

    bool input_terminator_t::terminating(const device_t &device) const
        { return dynamic_cast<const input_t &>(device).string().empty(); }

    string_t input_terminator_t::terminate(const device_t &device) const
    {
        if (not terminating(device))
            throw invalid_terminator_t(*this, device);

        return {};
    }

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

    bool input_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(input_t); }

    input_operation_scan_t::input_operation_scan_t(character_t character) :
        character_{character} {}

    input_operation_scan_t *input_operation_scan_t::clone() const
        { return new input_operation_scan_t{*this}; }

    bool input_operation_scan_t::applicable(const device_t &device) const
        { return dynamic_cast<const input_t &>(device).string().see(character_); }

    void input_operation_scan_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        dynamic_cast<input_t &>(device).string().pop();
        return;
    }

    bool input_operation_scan_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(input_operation_scan_t))
            return character_ ==
                dynamic_cast<const input_operation_scan_t &>(operation).character();
        if (typeid(operation) == typeid(input_operation_next_t))
            return character_ ==
                dynamic_cast<const input_operation_next_t &>(operation).character();
        if (typeid(operation) == typeid(input_operation_eof_t))
            return false;

        throw std::runtime_error(
                "In input_operation_scan_t::intersecting_domain(const operation_t &).");
    }

    bool input_operation_scan_t::intersecting_domain(const terminator_t &) const { return false; }

    character_t input_operation_scan_t::character() const { return character_; }

    input_operation_next_t::input_operation_next_t(character_t character) : character_{character} {}

    input_operation_next_t *input_operation_next_t::clone() const
        { return new input_operation_next_t{*this}; }

    bool input_operation_next_t::applicable(const device_t &device) const
        { return dynamic_cast<const input_t &>(device).string().see(character_); }

    void input_operation_next_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        return;
    }

    bool input_operation_next_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(input_operation_next_t))
            return character_ ==
                dynamic_cast<const input_operation_next_t &>(operation).character();
        if (typeid(operation) == typeid(input_operation_next_t))
            return character_ ==
                dynamic_cast<const input_operation_next_t &>(operation).character();
        if (typeid(operation) == typeid(input_operation_eof_t))
            return false;

        throw std::runtime_error(
                "In input_operation_next_t::intersecting_domain(const operation_t &).");
    }

    bool input_operation_next_t::intersecting_domain(const terminator_t &) const { return false; }

    character_t input_operation_next_t::character() const { return character_; }

    input_operation_eof_t *input_operation_eof_t::clone() const
        { return new input_operation_eof_t{*this}; }

    bool input_operation_eof_t::applicable(const device_t &device) const
        { return dynamic_cast<const input_t &>(device).string().empty(); }

    void input_operation_eof_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);
        return;
    }

    bool input_operation_eof_t::intersecting_domain(const operation_t &) const { return false; }
    bool input_operation_eof_t::intersecting_domain(const terminator_t &) const { return true; }
}

