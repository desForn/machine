#pragma once
#include "machine.hpp"

namespace Machine
{
    class stack_t;
    class stack_operation_t;
    class stack_operation_push_t;
    class stack_operation_pop_t;
    class stack_operation_top_t;
    class stack_operation_empty_t;
    class stack_initialiser_t;
    class stack_initialiser_empty_t;
    class stack_initialiser_string_t;
    class stack_terminator_t;
    class stack_terminator_empty_t;
    class stack_terminator_string_t;

    class stack_initialiser_t : public initialiser_t
    {
    public:
        virtual ~stack_initialiser_t() = default;
        stack_initialiser_t *clone() const override = 0;
    };

    class stack_initialiser_empty_t final : public stack_initialiser_t
    {
    public:
        stack_initialiser_empty_t *clone() const override;
    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class stack_initialiser_string_t final : public stack_initialiser_t
    {
    public:
        stack_initialiser_string_t *clone() const override;
    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class stack_terminator_t : public terminator_t
    {
    public:
        virtual ~stack_terminator_t() = default;
        stack_terminator_t *clone() const override = 0;
    };

    class stack_terminator_empty_t final : public stack_terminator_t
    {
    public:
        stack_terminator_empty_t *clone() const override;
    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class stack_terminator_string_t final : public stack_terminator_t
    {
    public:
        stack_terminator_string_t *clone() const override;
    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class stack_t final : public device_t
    {
    private:
        string_t string_{encoder().alphabet()};
        std::unique_ptr<stack_initialiser_t> initialiser_;
        std::unique_ptr<stack_terminator_t> terminator_;

    public:
        stack_t() = delete;
        ~stack_t () = default;

        stack_t(const stack_t &);
        stack_t &operator=(const stack_t &);

        stack_t(stack_t &&) noexcept = default;
        stack_t &operator=(stack_t &&) noexcept = default;

        stack_t(std::unique_ptr<encoder_t>,
                std::unique_ptr<stack_initialiser_t>, std::unique_ptr<stack_terminator_t>);
        stack_t *clone() const override;
            
    public:
        string_t &string();
        const string_t &string() const;
        const stack_initialiser_t &initialiser() const override;
        const stack_terminator_t &terminator() const override;
        std::string print_name() const override;
        std::string print_state() const override;
    };

    class stack_operation_t : public operation_t
    {
    public:
        virtual ~stack_operation_t() = default;

    public:
        bool correct_device(const device_t &) const override;
    };

    class stack_operation_push_t final: public stack_operation_t
    {
    private:
        character_t character_;

    public:
        stack_operation_push_t(character_t);
        stack_operation_push_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class stack_operation_pop_t final: public stack_operation_t
    {
    private:
        character_t character_;

    public:
        stack_operation_pop_t(character_t);
        stack_operation_pop_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class stack_operation_top_t final: public stack_operation_t
    {
    private:
        character_t character_;

    public:
        stack_operation_top_t(character_t);
        stack_operation_top_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;

        character_t character() const;
    };

    class stack_operation_empty_t final: public stack_operation_t
    {
    public:
        stack_operation_empty_t() = default;
        stack_operation_empty_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };
}

