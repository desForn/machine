#include "stack.hpp"

namespace Machine
{
    stack_initialiser_empty_t *stack_initialiser_empty_t::clone() const
        { return new stack_initialiser_empty_t{*this}; }

    void stack_initialiser_empty_t::initialise(device_t &device, const std::string &) const
        { dynamic_cast<stack_t &>(device).string() = string_t{device.encoder().alphabet()}; }

    stack_initialiser_string_t *stack_initialiser_string_t::clone() const
        { return new stack_initialiser_string_t{*this}; }
    
    void stack_initialiser_string_t::initialise(device_t &device, const std::string &string) const
        { dynamic_cast<stack_t &>(device).string() = device.encoder()(string); }

    stack_terminator_empty_t *stack_terminator_empty_t::clone() const
        { return new stack_terminator_empty_t{*this}; }

    bool stack_terminator_empty_t::terminating(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string().empty(); }

    std::string stack_terminator_empty_t::terminate(const device_t &) const
        { return {}; }

    stack_terminator_string_t *stack_terminator_string_t::clone() const
        { return new stack_terminator_string_t{*this}; }

    bool stack_terminator_string_t::terminating(const device_t &) const
        { return true; }

    std::string stack_terminator_string_t::terminate(const device_t &device) const
        { return device.encoder()(dynamic_cast<const stack_t &>(device).string()); }

    stack_t::stack_t(const stack_t &arg) :
        device_t{std::unique_ptr<encoder_t>{arg.encoder_->clone()}},
        string_{arg.string_},
        initialiser_{std::unique_ptr<stack_initialiser_t>{arg.initialiser_->clone()}},
        terminator_{std::unique_ptr<stack_terminator_t>{arg.terminator_->clone()}}
    {
        if (not encoder_)
            encoder_.reset(new encoder_ascii_t{});
    }

    stack_t &stack_t::operator=(const stack_t &arg) { return *this = stack_t{arg}; }

    stack_t::stack_t(std::unique_ptr<encoder_t> encoder_,
            std::unique_ptr<stack_initialiser_t> initialiser,
            std::unique_ptr<stack_terminator_t> terminator) :
        device_t{std::move(encoder_)},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    stack_t *stack_t::clone() const { return new stack_t{*this}; }

    string_t &stack_t::string() { return string_; }

    const string_t &stack_t::string() const { return string_; }

    const stack_initialiser_t &stack_t::initialiser() const { return *initialiser_; }

    const stack_terminator_t &stack_t::terminator() const { return *terminator_; }

    std::string stack_t::print_name() const { return "Stack"; }

    std::string stack_t::print_state() const { return string_.print_state(encoder()); }

    bool stack_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(stack_t); }

    stack_operation_push_t::stack_operation_push_t(character_t character) :
        character_{character} {}

    stack_operation_push_t *stack_operation_push_t::clone() const
        { return new stack_operation_push_t{*this}; }

    bool stack_operation_push_t::applicable(const device_t &) const { return true; }

    void stack_operation_push_t::apply(device_t &device) const
        { dynamic_cast<stack_t &>(device).string().push(character_); }

    bool stack_operation_push_t::intersecting_domain(const operation_t &) const { return true; }

    bool stack_operation_push_t::intersecting_domain(const terminator_t &) const { return true; }

    character_t stack_operation_push_t::character() const { return character_; }

    stack_operation_pop_t::stack_operation_pop_t(character_t character) :
        character_{character} {}

    stack_operation_pop_t *stack_operation_pop_t::clone() const
        { return new stack_operation_pop_t{*this}; }

    bool stack_operation_pop_t::applicable(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string().see(character_); }

    void stack_operation_pop_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        dynamic_cast<stack_t &>(device).string().pop();
        return;
    }

    bool stack_operation_pop_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(stack_operation_push_t))
            return true;
        if (typeid(operation) == typeid(stack_operation_pop_t))
            return character_ == dynamic_cast<const stack_operation_pop_t &>(operation).character();
        if (typeid(operation) == typeid(stack_operation_top_t))
            return character_ == dynamic_cast<const stack_operation_top_t &>(operation).character();
        if (typeid(operation) == typeid(stack_operation_empty_t))
            return false;
        if (typeid(operation) == typeid(noop_operation_t))
            return true;

        throw std::runtime_error{"In Machine::stack_operation_pop_t::intersecting_domain("
            "const operation_t &):\nUnknown operation.\n"};
    }

    bool stack_operation_pop_t::intersecting_domain(const terminator_t &terminator) const
    {
        if (typeid(terminator) == typeid(stack_terminator_empty_t))
            return false;
        if (typeid(terminator) == typeid(stack_terminator_string_t))
            return true;

        throw std::runtime_error{"In Machine::stack_operation_pop_t::intersecting_domain("
            "const terminator_t &):\nUnknown terminator.\n"};
    }

    character_t stack_operation_pop_t::character() const { return character_; }

    stack_operation_top_t::stack_operation_top_t(character_t character) :
        character_{character} {}

    stack_operation_top_t *stack_operation_top_t::clone() const
        { return new stack_operation_top_t{*this}; }

    bool stack_operation_top_t::applicable(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string().see(character_); }

    void stack_operation_top_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        return;
    }

    bool stack_operation_top_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(stack_operation_push_t))
            return true;
        if (typeid(operation) == typeid(stack_operation_pop_t))
            return character_ == dynamic_cast<const stack_operation_pop_t &>(operation).character();
        if (typeid(operation) == typeid(stack_operation_top_t))
            return character_ == dynamic_cast<const stack_operation_top_t &>(operation).character();
        if (typeid(operation) == typeid(stack_operation_empty_t))
            return false;
        if (typeid(operation) == typeid(noop_operation_t))
            return true;

        throw std::runtime_error{"In Machine::stack_operation_top_t::intersecting_domain("
            "const operation_t &):\nUnknown operation.\n"};
    }

    character_t stack_operation_top_t::character() const { return character_; }

    bool stack_operation_top_t::intersecting_domain(const terminator_t &terminator) const
    {
        if (typeid(terminator) == typeid(stack_terminator_empty_t))
            return false;
        if (typeid(terminator) == typeid(stack_terminator_string_t))
            return true;

        throw std::runtime_error{"In Machine::stack_operation_top_t::intersecting_domain("
            "const terminator_t &):\nUnknown terminator.\n"};
    }

    stack_operation_empty_t *stack_operation_empty_t::clone() const
        { return new stack_operation_empty_t{*this}; }

    bool stack_operation_empty_t::applicable(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string().empty(); }

    void stack_operation_empty_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);
        return;
    }

    bool stack_operation_empty_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(stack_operation_push_t))
            return true;
        if (typeid(operation) == typeid(stack_operation_pop_t))
            return false;
        if (typeid(operation) == typeid(stack_operation_top_t))
            return false;
        if (typeid(operation) == typeid(stack_operation_empty_t))
            return true;
        if (typeid(operation) == typeid(noop_operation_t))
            return true;

        throw std::runtime_error{"In Machine::stack_operation_empty_t::intersecting_domain("
            "const operation_t &):\nUnknown operation.\n"};
    }

    bool stack_operation_empty_t::intersecting_domain(const terminator_t &) const
        { return true; }
}

