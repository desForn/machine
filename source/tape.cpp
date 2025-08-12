#include "tape.hpp"

namespace Machine
{

    tape_initialiser_empty_t *tape_initialiser_empty_t::clone() const
        { return new tape_initialiser_empty_t{*this}; }

    void tape_initialiser_empty_t::initialise(device_t &device, const std::string &) const
    {
        tape_t &tape = dynamic_cast<tape_t &>(device);
        string_t &string = tape.string();
        string = string_t{tape.encoder().alphabet()};
        string.push(tape.default_character());

        return;
    }

    tape_initialiser_string_t *tape_initialiser_string_t::clone() const
        { return new tape_initialiser_string_t{*this}; }

    void tape_initialiser_string_t::initialise(device_t &device, const std::string &string) const
    {
        string_t &tape_string = dynamic_cast<tape_t &>(device).string();
        tape_string = device.encoder()(string);
        tape_string.pos() = 0;

        return;
    }

    tape_terminator_athome_t *tape_terminator_athome_t::clone() const
        { return new tape_terminator_athome_t{*this}; }

    bool tape_terminator_athome_t::terminating(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string().athome(); }

    std::string tape_terminator_athome_t::terminate(const device_t &device) const 
    {
        const tape_t &tape = dynamic_cast<const tape_t &>(device);
        string_t string = tape.string();

        string.pos() = std::size(string) - 1;
        while (not std::empty(string) and string.see(tape.default_character()))
            string.pop();

        return tape.encoder()(string);
    }

    tape_terminator_always_t *tape_terminator_always_t::clone() const
        { return new tape_terminator_always_t{*this}; }

    bool tape_terminator_always_t::terminating(const device_t &) const { return true; }

    std::string tape_terminator_always_t::terminate(const device_t &device) const
    {
        const tape_t &tape = dynamic_cast<const tape_t &>(device);
        string_t string = tape.string();

        string.pos() = std::size(string) - 1;
        while (not std::empty(string) and string.see(tape.default_character()))
            string.pop();

        return tape.encoder()(string);
    }

    tape_t::tape_t(const tape_t &arg) :
        device_t{std::unique_ptr<encoder_t>{arg.encoder().clone()}},
        string_{arg.string_},
        default_character_{arg.default_character_},
        initialiser_{arg.initialiser_->clone()},
        terminator_{arg.terminator_->clone()} {}

    tape_t &tape_t::operator=(const tape_t &arg) { return *this = tape_t{arg}; }

    tape_t::tape_t(std::unique_ptr<encoder_t> encoder_, character_t default_character,
            std::unique_ptr<tape_initialiser_t> initialiser,
            std::unique_ptr<tape_terminator_t> terminator) :
        device_t{std::move(encoder_)},
        default_character_{default_character},
        initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)}
    {
        if (default_character > string_.alphabet().max_character())
            throw std::runtime_error{"In Machine::tape_t::tape_t(std::unique_ptr<encoder_t>, "
                "character_t, std::unique_ptr<tape_initialiser_t>, "
                "std::unique_ptr<tape_terminator_t>):\nInvalid default character.\n"};

        string_.push(default_character_);

        return;
    }

    tape_t *tape_t::clone() const { return new tape_t{*this}; }

    character_t tape_t::default_character() const { return default_character_; }

    string_t &tape_t::string() { return string_; }
    const string_t &tape_t::string() const { return string_; }
    const tape_initialiser_t &tape_t::initialiser() const { return *initialiser_; }
    const tape_terminator_t &tape_t::terminator() const { return *terminator_; }
    std::string tape_t::print_state() const
    {
        std::string ret = encoder()(string_);
        ret.push_back('\n');
        ret.resize(std::size(ret) + string_.pos(), ' ');
        ret.push_back('^');
        return ret;
    }

    bool tape_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(tape_t); }

    tape_operation_see_t::tape_operation_see_t(character_t character) : character_{character} {}

    tape_operation_see_t *tape_operation_see_t::clone() const
        { return new tape_operation_see_t{*this}; }

    bool tape_operation_see_t::correct_device(const device_t &device) const
    {
        return typeid(device) == typeid(tape_t) and
            character_ <= device.encoder().alphabet().max_character();
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
            character_ <= device.encoder().alphabet().max_character();
    }

    bool tape_operation_print_t::applicable(const device_t &) const { return true; }

    void tape_operation_print_t::apply(device_t &device) const
        { dynamic_cast<tape_t &>(device).string().print(character_); }

    bool tape_operation_print_t::intersecting_domain(const operation_t &) const { return true; }
    bool tape_operation_print_t::intersecting_domain(const terminator_t &) const { return true; }

    character_t tape_operation_print_t::character() const { return character_; }

    tape_operation_move_l_t *tape_operation_move_l_t::clone() const
        { return new tape_operation_move_l_t{*this}; }

    bool tape_operation_move_l_t::applicable(const device_t &device) const
        { return dynamic_cast<const tape_t &>(device).string().pos() != 0; }

    void tape_operation_move_l_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t{*this, device};

        dynamic_cast<tape_t &>(device).string().move_l();

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
        tape.string().move_r(tape.default_character());
        return;
    }

    bool tape_operation_move_r_t::intersecting_domain(const operation_t &) const { return true; }

    bool tape_operation_move_r_t::intersecting_domain(const terminator_t &) const { return true; }

    tape_operation_athome_t *tape_operation_athome_t::clone() const
        { return new tape_operation_athome_t{*this}; }

    bool tape_operation_athome_t::applicable(const device_t &device) const
    { return dynamic_cast<const tape_t &>(device).string().athome(); }

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
        static const std::string error{"In Machine::tape_operation_compound_t(std::array<"
            "std::shared_ptr<tape_operation_t>, 4>):\n"};

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

        if (operations_[0] and not string.athome())
            return false;

        if (operations_[1] and not string.see(
                    dynamic_cast<const tape_operation_see_t &>(*operations_[1]).character()))
            return false;

        if (operations_[3] and typeid(*operations_[3]) == typeid(tape_operation_move_l_t) and
                string.athome())
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

        throw std::runtime_error{"In Machine::tape_operation_compound_t::intersecting_domain("
            "const operation_t &).\nInvalid typeid of the argument.\n"};
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

