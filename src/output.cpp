#include "output.hpp"

namespace Machine
{
    output_initialiser_t *output_initialiser_t::clone() const
        { return new output_initialiser_t{*this}; }

    void output_initialiser_t::initialise(device_t &device, const std::string &) const
        { dynamic_cast<output_t &>(device).string() = string_t{device.encoder().alphabet()}; }

    output_terminator_t *output_terminator_t::clone() const
        { return new output_terminator_t{*this}; }

    bool output_terminator_t::terminating(const device_t &) const { return true; }

    std::string output_terminator_t::terminate(const device_t &device) const
    {
        return dynamic_cast<const output_t &>(device).string().
            print_state(device.encoder());
    }

    output_initialiser_t output_t::initialiser_{};
    output_terminator_t output_t::terminator_{};

    output_t::output_t(const output_t &arg) :
        device_t{std::unique_ptr<encoder_t>{arg.encoder_->clone()}}, string_{arg.string_} {}

    output_t &output_t::operator=(const output_t &arg) { return *this = output_t{arg}; }

    output_t::output_t(std::unique_ptr<encoder_t> encoder) : device_t{std::move(encoder)}
    {
        if (not encoder_)
            encoder_.reset(new encoder_ascii_t{});
    }

    output_t *output_t::clone() const { return new output_t{*this}; }

    const output_initialiser_t &output_t::initialiser() const { return initialiser_; }

    const output_terminator_t &output_t::terminator() const { return terminator_; }

    std::string output_t::print_name() const { return "Output"; }

    std::string output_t::print_state() const { return string_.print_state(encoder()); }

    string_t &output_t::string() { return string_; }

    const string_t &output_t::string() const { return string_; }

    output_operation_write_t::output_operation_write_t(character_t character):
        character_{character} {}

    output_operation_write_t *output_operation_write_t::clone() const
        { return new output_operation_write_t{*this}; }

    bool output_operation_write_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(output_t); }

    bool output_operation_write_t::applicable(const device_t &) const { return true; }

    void output_operation_write_t::apply(device_t &device) const
        { dynamic_cast<output_t &>(device).string().push(character_); }

    bool output_operation_write_t::intersecting_domain(const operation_t &) const { return true; }
    bool output_operation_write_t::intersecting_domain(const terminator_t &) const { return true; }

    character_t output_operation_write_t::character() const { return character_; }
}

