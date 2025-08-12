#pragma once
#include "machine.hpp"

namespace Machine
{
    class tape_t;
    class tape_operation_t;
    class tape_operation_see_t;
    class tape_operation_print_t;
    class tape_operation_move_l_t;
    class tape_operation_move_r_t;
    class tape_operation_athome_t;
    class tape_operation_compound_t;
    class tape_initialiser_t;
    class tape_initialiser_empty_t;
    class tape_initialiser_string_t;
    class tape_terminator_t;
    class tape_terminator_athome_t;
    class tape_terminator_always_t;

    class tape_initialiser_t : public initialiser_t
    {
    public:
        virtual ~tape_initialiser_t() = default;
        tape_initialiser_t *clone() const override = 0;
    };

    class tape_initialiser_empty_t final : public tape_initialiser_t
    {
    public:
        tape_initialiser_empty_t *clone() const override;
    public:
        virtual void initialise(device_t &, const std::string &) const override;
    };

    class tape_initialiser_string_t final : public tape_initialiser_t
    {
    public:
        tape_initialiser_string_t *clone() const override;
    public:
        virtual void initialise(device_t &, const std::string &) const override;
    };

    class tape_terminator_t : public terminator_t
    {
    public:
        virtual ~tape_terminator_t() = default;
        tape_terminator_t *clone() const override = 0;
    };

    class tape_terminator_athome_t final : public tape_terminator_t
    {
    public:
        tape_terminator_athome_t *clone() const override;
    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class tape_terminator_always_t final : public tape_terminator_t
    {
    public:
        tape_terminator_always_t *clone() const override;
    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class tape_t final : public device_t
    {
    private:
        string_t string_{encoder().alphabet()};
        character_t default_character_;
        std::unique_ptr<tape_initialiser_t> initialiser_;
        std::unique_ptr<tape_terminator_t> terminator_;

    public:
        tape_t() = delete;
        ~tape_t() = default;

        tape_t(const tape_t &);
        tape_t &operator=(const tape_t &);

        tape_t(tape_t &&) noexcept = default;
        tape_t &operator=(tape_t &&) noexcept = default;

        tape_t(std::unique_ptr<encoder_t>, character_t,
                std::unique_ptr<tape_initialiser_t>, std::unique_ptr<tape_terminator_t>);
        tape_t *clone() const override;
            
    public:
        character_t default_character() const;
        string_t &string();
        const string_t &string() const;
        const tape_initialiser_t &initialiser() const override;
        const tape_terminator_t &terminator() const override;
        std::string print_state() const override;
    };

    class tape_operation_t : public operation_t
    {
    public:
        virtual ~tape_operation_t() = default;

    public:
        bool correct_device(const device_t &) const override;
    };

    class tape_operation_see_t final : public tape_operation_t
    {
    private:
        character_t character_;

    public:
        tape_operation_see_t(character_t);
        tape_operation_see_t *clone() const override;

    public:
        bool correct_device(const device_t &) const override;
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class tape_operation_print_t final : public tape_operation_t
    {
    private:
        character_t character_;

    public:
        tape_operation_print_t(character_t);
        tape_operation_print_t *clone() const override;

    public:
        bool correct_device(const device_t &) const override;
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class tape_operation_move_l_t final : public tape_operation_t
    {
    public:
        tape_operation_move_l_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class tape_operation_move_r_t final : public tape_operation_t
    {
    public:
        tape_operation_move_r_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class tape_operation_athome_t final : public tape_operation_t
    {
    public:
        tape_operation_athome_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class tape_operation_compound_t final : public tape_operation_t
    {
    private:
        std::array<std::shared_ptr<operation_t>, 4> operations_;

    public:
        tape_operation_compound_t(std::array<std::shared_ptr<operation_t>, 1>);
        tape_operation_compound_t(std::array<std::shared_ptr<operation_t>, 2>);
        tape_operation_compound_t(std::array<std::shared_ptr<operation_t>, 3>);
        tape_operation_compound_t(std::array<std::shared_ptr<operation_t>, 4>);
        tape_operation_compound_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        const std::array<std::shared_ptr<operation_t>, 4> operations() const;
    };
}

