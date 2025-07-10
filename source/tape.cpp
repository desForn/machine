#include "tape.hpp"

namespace Machine
{

    tape_initialiser_empty_t *tape_initialiser_empty_t::clone() const
        { return new tape_initialiser_empty_t{*this}; }

    void tape_initialiser_empty_t::initialise(device_t &device, const string_t &) const
        { dynamic_cast<tape_t &>(device).string().clear(); }

    tape_initialiser_string_t *tape_initialiser_string_t::clone() const
        { return new tape_initialiser_string_t{*this}; }

    void tape_initialiser_string_t::initialise(device_t &device, const string_t &string) const
    {
        if (dynamic_cast<tape_t &>(device).alphabet() != string.alphabet())
            throw invalid_alphabet_t{};

        dynamic_cast<tape_t &>(device).string() = string;

        return;
    }

    tape_terminator_zero_t *tape_terminator_zero_t::clone() const
        { return new tape_terminator_zero_t{*this}; }

    bool tape_terminator_empty_t::terminating(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string().empty(); }

    string_t tape_terminator_empty_t(const device_t &device) const
        { return {dynamic_cast<const tape_t &>(device).alphabet()}; }

    tape_terminator_string_t *tape_terminator_string_t::clone() const
        { return new tape_terminator_string_t{*this}; }

    bool tape_terminator_string_t::terminating(const device_t &) const { return true; }

    string_t tape_terminator_string_t(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string(); }

    tape_t::tape_t(const tape_t &arg) :
        string_{arg.string_},
        default_character_{arg.default_character_},
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()}

    tape_t &operator=(const tape_t &arg)
    {
        string_ = arg.string_;
        default_character_ = arg.default_character_;
        initialiser_.reset(arg.initialiser_->clone());
        terminator_.reset(arg.terminator_->clone());
        return *this;
    }

    tape_t::tape_t(const alphabet_t &alphabet, character_t default_character,
            const tape_initialiser_t &initialiser, const tape_terminator_t &terminator) :
        string_{alphabet},
        default_character_{default_character},
        initialiser_{initialiser.clone()},
        terminator_{terminator.clone()}
    {
        if (default_character >= string_.alphabet().n_characters)
            throw invalid_character_t{};

        string_.push(default_character_);

        return;
    }

    tape_t::tape_t(const alphabet_t &alphabet, character_t default_character,
            std::unique_ptr<tape_initialiser_t> &initialiser
            std::unique_ptr<tape_terminator_t> &terminator) :
        string_{alphabet},
        default_character_{default_character},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)}
    {
        if (default_character >= string_.alphabet().n_characters)
            throw invalid_character_t{};

        string_.push(default_character_);

        return;
    }

    tape_t *tape_t::clone() const { return new tape_t{*this}; }

    const alphabet_t &tape_t::alphabet() const { return string_.alphabet(); }

    string_t &tape_t::string() { return string_; }
    const string_t &tape_t::string() const { return string_; }
    const tape_intialiser_t &initialiser() const { return *initialiser_; }
    const tape_terminator_t &terminator() const { return *terminator_; }

    bool tape_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(tape_t); }

    tape_operation_see_t::tape_operation_see_t(character_t character) : character_{character} {}

    tape_operation_see_t *tape_operation_see_t::clone() const
        { return new tape_operation_see_t{*this}; }

    bool tape_operation_see_t::correct_device(const device_t &device) const
    {
        return typeid(device) == typeid(tape_t) and
            character_ < dynamic_cast<const tape_t &>(device).alphabet().n_characters();
    }

    bool tape_operation_see_t::applicable(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string().see() == character_; }

    void tape_operation_see_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation{*this, device};
        return;
    }

    bool tape_operation_see_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(tape_operation_see_t))
            return character_ == dynamic_cast<const tape_operation_see_t &>(operation).character_;
        return true;
    }

    bool tape_operation_see_t::intersecting_domain(const terminator_t &terminator) const
        { return true; }

    tape_operation_print_t::tape_operation_print_t(character_t character) : character_{character} {}

    tape_operation_print_t *tape_operation_print_t::clone() const
        { return new tape_operation_print_t{*this}; }

    bool tape_operation_print_t::correct_device(const device_t &device) const
    {
        return typeid(device) == typeid(tape_t) and
            character_ < dynamic_cast<const tape_t &>(device).alphabet().n_characters();
    }

    bool tape_operation_print_t::applicable(const device_t &device) const { return true; }

    void tape_operation_print_t::apply(device_t &device) const
    {
        dynamic_cast<tape_t &>(device).string().print(character_);
        return;
    }

    bool tape_operation_print_t::intersecting_domain(const operation_t &operation) const
        { return true; }

    bool tape_operation_print_t::intersecting_domain(const terminator_t &terminator) const
        { return true; }

    tape_operation_move_l_t *tape_operation_move_l_t::clone() const
        { return new tape_operation_move_l_t{*this}; }

    bool tape_operation_move_l::applicable(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string().get_pos() != 0; }

    void tape_operation_move_l::apply(device_t &device) const
    {
        index_t pos = dynamic_cast<tape_t &>(device).get_pos();
        
        if (pos == 0)
            throw invalid_operation{*this, device};

        dynamic_cast<tape_t &>(device).set_pos(--pos);

        return;
    }

    bool tape_operation_move_l::intersecting_domain(const operation_t &operation) const
    { return typeid(operation) != typeid(tape_operation_athome_t) and }

    bool tape_operation_move_l::intersecting_domain(const terminator_t &terminator) const
        { return typeid(teminator) != typeid(tape_terminator_empty_t); }

    tape_operation_move_r_t *tape_operation_move_r_t::clone() const
        { return new tape_operation_move_r_t{*this}; }

    bool tape_operation_move_r::applicable(const device_t &device) const { return true; }

    void tape_operation_move_r::apply(device_t &device) const
    {
        tape_t &tape = dynamic_cast<tape_t &>(device);

        if (tape.get_pos() == std::size(tape.data()) - 1)
            tape.string.push(default_character);

        else
            tape.set_pos(++tape.get_pos());

        return;
    }

    bool tape_operation_move_r::intersecting_domain(const operation_t &) const
        { return true; }

    bool tape_operation_move_r::intersecting_domain(const terminator_t &) const
        { return true; }

    tape_operation_athome_t *tape_operation_athome_t::clone() const
        { return new tape_operation_athome_t{*this}; }

    bool tape_operation_athome::applicable(const device_t &device) const
    { return dynamic_cast<const tape_t &>(device).string().get_pos() == 0; }

    void tape_operation_athome::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        return;
    }

    bool tape_operation_athome::intersecting_domain(const operation_t &operation) const
        { return typeid(operation) != typeid(tape_operation_move_l); }

    bool tape_operation_athome::intersecting_domain(const terminator_t &) const
        { return true; }
}

