#include "tape.hpp"

namespace Machine
{

    tape_initialiser_empty_t *tape_initialiser_empty_t::clone() const
        { return new tape_initialiser_empty_t{*this}; }

    void tape_initialiser_empty_t::initialise(device_t &device, const string_t &) const
    {
        tape_t &tape = dynamic_cast<tape_t &>(device);
        string_t &string = tape.string();
        string.clear();
        string.push(tape.default_character());

        return;
    }

    tape_initialiser_string_t *tape_initialiser_string_t::clone() const
        { return new tape_initialiser_string_t{*this}; }

    void tape_initialiser_string_t::initialise(device_t &device, const string_t &string) const
    {
        if (dynamic_cast<tape_t &>(device).alphabet() != string.alphabet())
            throw invalid_alphabet_t{};

        tape_t &tape = dynamic_cast<tape_t &>(device);
        string_t &string_ = tape.string();
        string_.clear();
        for (auto it = std::crbegin(string.data()); it != std::crend(string.data()); ++it)
            string_.push(*it);
        if (std::empty(string_))
            string_.push(tape.default_character());
        string_.set_pos(0);

        return;
    }

    tape_terminator_athome_t *tape_terminator_athome_t::clone() const
        { return new tape_terminator_athome_t{*this}; }

    bool tape_terminator_athome_t::terminating(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string().get_pos() == 0; }

    string_t tape_terminator_athome_t::terminate(const device_t &device) const
        { return {dynamic_cast<const tape_t &>(device).alphabet()}; }

    tape_terminator_always_t *tape_terminator_always_t::clone() const
        { return new tape_terminator_always_t{*this}; }

    bool tape_terminator_always_t::terminating(const device_t &) const { return true; }

    string_t tape_terminator_always_t::terminate(const device_t &device) const
    {
        const tape_t &tape = dynamic_cast<const tape_t &>(device);
        const string_t &string = tape.string();

        string_t ret{string.alphabet()};

        auto it = std::crbegin(string.data());

        while (it != std::crend(string.data()) and *it == tape.default_character())
            ++it;

        for (; it != std::crend(string.data()); ++it)
            ret.push(*it);

        return ret;
    }

    tape_t::tape_t(const tape_t &arg) :
        string_{arg.string_},
        default_character_{arg.default_character_},
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()} {}

    tape_t &tape_t::operator=(const tape_t &arg)
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
        if (default_character >= string_.alphabet().n_characters())
            throw invalid_character_t{};

        string_.push(default_character_);

        return;
    }

    tape_t::tape_t(const alphabet_t &alphabet, character_t default_character,
            std::unique_ptr<tape_initialiser_t> initialiser,
            std::unique_ptr<tape_terminator_t> terminator) :
        string_{alphabet},
        default_character_{default_character},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)}
    {
        if (default_character >= string_.alphabet().n_characters())
            throw invalid_character_t{};

        string_.push(default_character_);

        return;
    }

    tape_t *tape_t::clone() const { return new tape_t{*this}; }

    const alphabet_t &tape_t::alphabet() const { return string_.alphabet(); }

    character_t tape_t::default_character() const { return default_character_; }

    string_t &tape_t::string() { return string_; }
    const string_t &tape_t::string() const { return string_; }
    const tape_initialiser_t &tape_t::initialiser() const { return *initialiser_; }
    const tape_terminator_t &tape_t::terminator() const { return *terminator_; }

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
        { return dynamic_cast<const tape_t &>(device).string().see(character_); }

    void tape_operation_see_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};
        return;
    }

    bool tape_operation_see_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(tape_operation_compound_t))
            return operation.intersecting_domain(*this);

        if (typeid(operation) == typeid(tape_operation_see_t))
            return character_ == dynamic_cast<const tape_operation_see_t &>(operation).character_;
        return true;
    }

    bool tape_operation_see_t::intersecting_domain(const terminator_t &) const { return true; }

    character_t tape_operation_see_t::character() const { return character_; }

    tape_operation_print_t::tape_operation_print_t(character_t character) : character_{character} {}

    tape_operation_print_t *tape_operation_print_t::clone() const
        { return new tape_operation_print_t{*this}; }

    bool tape_operation_print_t::correct_device(const device_t &device) const
    {
        return typeid(device) == typeid(tape_t) and
            character_ < dynamic_cast<const tape_t &>(device).alphabet().n_characters();
    }

    bool tape_operation_print_t::applicable(const device_t &) const { return true; }

    void tape_operation_print_t::apply(device_t &device) const
    {
        dynamic_cast<tape_t &>(device).string().print(character_);
        return;
    }

    bool tape_operation_print_t::intersecting_domain(const operation_t &) const { return true; }
    bool tape_operation_print_t::intersecting_domain(const terminator_t &) const { return true; }

    character_t tape_operation_print_t::character() const { return character_; }

    tape_operation_move_l_t *tape_operation_move_l_t::clone() const
        { return new tape_operation_move_l_t{*this}; }

    bool tape_operation_move_l_t::applicable(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string().get_pos() != 0; }

    void tape_operation_move_l_t::apply(device_t &device) const
    {
        index_t pos = dynamic_cast<tape_t &>(device).string().get_pos();
        
        if (pos == 0)
            throw invalid_operation_t{*this, device};

        dynamic_cast<tape_t &>(device).string().set_pos(--pos);

        return;
    }

    bool tape_operation_move_l_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(tape_operation_compound_t))
            return operation.intersecting_domain(*this);

        return typeid(operation) != typeid(tape_operation_athome_t);
    }

    bool tape_operation_move_l_t::intersecting_domain(const terminator_t &terminator) const
        { return typeid(terminator) != typeid(tape_terminator_athome_t); }

    tape_operation_move_r_t *tape_operation_move_r_t::clone() const
        { return new tape_operation_move_r_t{*this}; }

    bool tape_operation_move_r_t::applicable(const device_t &) const { return true; }

    void tape_operation_move_r_t::apply(device_t &device) const
    {
        tape_t &tape = dynamic_cast<tape_t &>(device);

        if (tape.string().get_pos() == std::size(tape.string().data()) - 1)
            tape.string().push(tape.default_character());

        else
            tape.string().set_pos(tape.string().get_pos() + 1);

        return;
    }

    bool tape_operation_move_r_t::intersecting_domain(const operation_t &) const
        { return true; }

    bool tape_operation_move_r_t::intersecting_domain(const terminator_t &) const
        { return true; }

    tape_operation_athome_t *tape_operation_athome_t::clone() const
        { return new tape_operation_athome_t{*this}; }

    bool tape_operation_athome_t::applicable(const device_t &device) const
    { return dynamic_cast<const tape_t &>(device).string().get_pos() == 0; }

    void tape_operation_athome_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        return;
    }

    bool tape_operation_athome_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(tape_operation_compound_t))
            return operation.intersecting_domain(*this);

        return typeid(operation) != typeid(tape_operation_move_l_t);
    }

    bool tape_operation_athome_t::intersecting_domain(const terminator_t &) const { return true; }

    tape_operation_compound_t::tape_operation_compound_t(
            std::array<std::shared_ptr<operation_t>, 1> arg) :
        tape_operation_compound_t{std::array<std::shared_ptr<operation_t>, 4>{
            std::move(arg[0]), nullptr, nullptr, nullptr}} {}

    tape_operation_compound_t::tape_operation_compound_t(
            std::array<std::shared_ptr<operation_t>, 2> arg) :
        tape_operation_compound_t{std::array<std::shared_ptr<operation_t>, 4>{
            std::move(arg[0]), std::move(arg[1]), nullptr, nullptr}} {}

    tape_operation_compound_t::tape_operation_compound_t(
            std::array<std::shared_ptr<operation_t>, 3> arg) :
        tape_operation_compound_t{std::array<std::shared_ptr<operation_t>, 4>{
            std::move(arg[0]), std::move(arg[1]), std::move(arg[2]), nullptr}} {}

    tape_operation_compound_t::tape_operation_compound_t(
            std::array<std::shared_ptr<operation_t>, 4> arg)
    {
        static const std::string error{"In tape_operation_compound_t(std::array<std::shared_ptr<"
            "tape_operation_t>, 4>):\n"};

        for (std::shared_ptr<operation_t> &i : arg)
            if (i and typeid(*i) == typeid(tape_operation_athome_t))
            {
                if (operations_[0])
                    throw std::runtime_error{error + "\tMultiple athome operations.\n"};
                operations_[0] = std::move(i);
            }

        for (std::shared_ptr<operation_t> &i : arg)
            if (i and typeid(*i) == typeid(tape_operation_see_t))
            {
                if (operations_[1])
                    throw std::runtime_error{error + "\tMultiple see operations.\n"};
                operations_[1] = std::move(i);
            }

        for (std::shared_ptr<operation_t> &i : arg)
            if (i and typeid(*i) == typeid(tape_operation_print_t))
            {
                if (operations_[2])
                    throw std::runtime_error{error + "\tMultiple print operations.\n"};
                operations_[2] = std::move(i);
            }

        for (std::shared_ptr<operation_t> &i : arg)
            if (i and (typeid(*i) == typeid(tape_operation_move_l_t) or
                typeid(*i) == typeid(tape_operation_move_r_t)))
            {
                if (operations_[3])
                    throw std::runtime_error{error + "\tMultiple move operations.\n"};
                operations_[3] = std::move(i);
            }

        if (operations_[0] and operations_[3] and
                typeid(operations_[3]) == typeid(tape_operation_move_l_t))
            throw std::runtime_error{error + "\tIncompatible operation: athome and move_l.\n"};

        return;
    }

    tape_operation_compound_t *tape_operation_compound_t::clone() const
        { return new tape_operation_compound_t{*this}; }

    bool tape_operation_compound_t::applicable(const device_t &device) const
    {
        const string_t &string = dynamic_cast<const tape_t &>(device).string();

        if (operations_[0] and string.get_pos() != 0)
            return false;

        if (operations_[1] and not string.see(
                    dynamic_cast<const tape_operation_see_t &>(*operations_[1]).character()))
            return false;

        if (operations_[3] and typeid(*operations_[3]) == typeid(tape_operation_move_l_t) and
                string.get_pos() == 0)
            return false;

        return true;
    }

    void tape_operation_compound_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        for (const std::shared_ptr<operation_t> &i : operations_)
            if (i)
                i->apply(device);

        return;
    }

    bool tape_operation_compound_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(tape_operation_see_t))
            return operations_[1] and dynamic_cast<const tape_operation_see_t &>(
                *operations_[1]).character() == dynamic_cast<const tape_operation_see_t &>(
                operation).character();

        if (typeid(operation) == typeid(tape_operation_print_t))
            return true;

        if (typeid(operation) == typeid(tape_operation_move_l_t))
            return not operations_[0];

        if (typeid(operation) == typeid(tape_operation_move_r_t))
            return true;

        if (typeid(operation) == typeid(tape_operation_athome_t))
            return not operations_[3] or typeid(*operations_[3]) != typeid(tape_operation_move_l_t);

        if (typeid(operation) == typeid(tape_operation_compound_t))
        {
            const tape_operation_compound_t &compound =
                dynamic_cast<const tape_operation_compound_t &>(operation);

            if (operations_[0] and compound.operations_[3] and
                typeid(*compound.operations_[3]) == typeid(tape_operation_move_l_t))
                return false;

            if (compound.operations_[0] and operations_[3] and
                typeid(*operations_[3]) == typeid(tape_operation_move_l_t))
                return false;

            return operations_[1] and compound.operations_[1] and
                operations_[1]->intersecting_domain(*compound.operations_[1]);
        }
            
        if (typeid(operation) == typeid(noop_operation_t))
            return true;

        throw std::runtime_error{"In tape_operation_compound_t::intersecting_domain(const operation"
            "_t &).\nInvalid typeid of the argument.\n"};
    }

    bool tape_operation_compound_t::intersecting_domain(const terminator_t &terminator) const
    {
        return typeid(terminator) == typeid(tape_terminator_always_t) or
            (typeid(terminator) == typeid(tape_terminator_athome_t) and
             not (operations_[3] and typeid(*operations_[3]) == typeid(tape_operation_move_l_t)));
    }

    const std::array<std::shared_ptr<operation_t>, 4> tape_operation_compound_t::operations() const
        { return operations_; }
}

