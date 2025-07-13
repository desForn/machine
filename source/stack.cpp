#include "stack.hpp"

namespace Machine
{
    stack_initialiser_empty_t *stack_initialiser_empty_t::clone() const
        { return new stack_initialiser_empty_t{*this}; }
    void stack_initialiser_empty_t::initialise(device_t &device, const string_t &) const
        { dynamic_cast<stack_t &>(device).string().clear(); }

    stack_initialiser_string_t *stack_initialiser_string_t::clone() const
        { return new stack_initialiser_string_t{*this}; }
    void stack_initialiser_string_t::initialise(device_t &device, const string_t &string) const
    {
        string_t &string_ = dynamic_cast<stack_t &>(device).string();
        if (string_.alphabet() != string.alphabet())
            throw std::runtime_error(
                    "In stack_initialiser_t::initialise(device_t &, cons string_t &)");
        string_ = string;
        return;
    }

    stack_terminator_empty_t *stack_terminator_empty_t::clone() const
        { return new stack_terminator_empty_t{*this}; }
    bool stack_terminator_empty_t::terminating(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string().empty(); }
    string_t stack_terminator_empty_t::terminate(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string(); }

    stack_terminator_string_t *stack_terminator_string_t::clone() const
        { return new stack_terminator_string_t{*this}; }
    bool stack_terminator_string_t::terminating(const device_t &) const
        { return true; }
    string_t stack_terminator_string_t::terminate(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string(); }

    stack_t::stack_t(const stack_t &arg) :
        string_{arg.string_},
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()} {}

    stack_t &stack_t::operator=(const stack_t &arg)
    {
        string_ = arg.string_;
        initialiser_.reset(arg.initialiser_->clone());
        terminator_.reset(arg.terminator_->clone());
        return *this;
    }

    stack_t::stack_t(const alphabet_t &alphabet,
            const stack_initialiser_t &initialiser, const stack_terminator_t &terminator) :
        string_{alphabet},
        initialiser_{initialiser.clone()},
        terminator_{terminator.clone()} {}

    stack_t::stack_t(const alphabet_t &alphabet,
            std::unique_ptr<stack_initialiser_t> initialiser,
            std::unique_ptr<stack_terminator_t> terminator) :
        string_{alphabet},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    stack_t *stack_t::clone() const { return new stack_t{*this}; }

    const alphabet_t &stack_t::alphabet() const { return string_.alphabet(); }
    string_t &stack_t::string() { return string_; }
    const string_t &stack_t::string() const { return string_; }
    const stack_initialiser_t &stack_t::initialiser() const { return *initialiser_; }
    const stack_terminator_t &stack_t::terminator() const { return *terminator_; }

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
        { return dynamic_cast<const stack_t &>(device).string().see() == character_; }
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

        throw std::runtime_error(
                "In stack_operation_pop_t::intersecting_domain(const operation_t &).");
    }

    bool stack_operation_pop_t::intersecting_domain(const terminator_t &terminator) const
    {
        if (typeid(terminator) == typeid(stack_terminator_empty_t))
            return false;
        if (typeid(terminator) == typeid(stack_terminator_string_t))
            return true;

        throw std::runtime_error(
                "In stack_operation_pop_t::intersecting_domain(const terminator_t &).");
    }

    character_t stack_operation_pop_t::character() const { return character_; }

    stack_operation_top_t::stack_operation_top_t(character_t character) :
        character_{character} {}
    stack_operation_top_t *stack_operation_top_t::clone() const
        { return new stack_operation_top_t{*this}; }

    bool stack_operation_top_t::applicable(const device_t &device) const
        { return dynamic_cast<const stack_t &>(device).string().see() == character_; }
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

        throw std::runtime_error(
                "In stack_operation_top_t::intersecting_domain(const operation_t &).");
    }

    character_t stack_operation_top_t::character() const { return character_; }

    bool stack_operation_top_t::intersecting_domain(const terminator_t &terminator) const
    {
        if (typeid(terminator) == typeid(stack_terminator_empty_t))
            return false;
        if (typeid(terminator) == typeid(stack_terminator_string_t))
            return true;

        throw std::runtime_error(
                "In stack_operation_top_t::intersecting_domain(const terminator_t &).");
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

        throw std::runtime_error(
                "In stack_operation_empty_t::intersecting_domain(const operation_t &).");
    }

    bool stack_operation_empty_t::intersecting_domain(const terminator_t &) const
        { return true; }
}

