#pragma once
#include "string.hpp"

#include <memory>
#include <span>

namespace Machine
{
    class machine_t;
    class device_t;
    class initialiser_t;
    class terminator_t;
    class operation_t;
    class noop_t;

    class invalid_operation;
    class invalid_terminator;

    class control_t;

    class machine_t
    {
    public:
        enum class machine_state_t
            { invalid, running, non_deterministic_decision, blocked, halted };

    private:
        std::vector<std::unique_ptr<device_t>> devices_;
        std::vector<std::unique_ptr<operation_t>> instruction_set_;
        std::vector<std::unique_ptr<operation_t>> applicable_instructions_{};
        std::vector<std::vector<std::unique_ptr<operation_t>>::iterator> search_table_{};
        std::vector<string_t> output_{};
        control_t *first_control_{nullptr};
        machine_state_t state_{machine_state_t::invalid};
        bool deterministic_ = true;

    public:
        machine_t(std::vector<std::unique_ptr<device_t>>,
                  std::vector<std::unique_ptr<operation_t>>);
        virtual ~machine_t() = default;

    public:
        decltype(auto) devices(this auto &&self)
            { return std::span(std::begin(self.devices_), std::end(self.devices_)); };

        decltype(auto) instruction_set(this auto &&self)
            { return std::span(std::begin(self.instruction_set_),
                               std::end(self.instruction_set_)); };

        std::span<const string_t> output() const;
        bool deterministic() const;
        machine_state_t state() const;

    public:
        void initialise(const string_t &);
        void next();
        void next(index_t);
        void run();
        std::vector<std::span<const std::unique_ptr<operation_t>>> applicable_instructions() const;
        bool terminating() const;
    private:
        void terminate();
        void applicable_instructions_apparatus();
    };
    
    class device_t
    {
    public:
        virtual ~device_t() = default;
        virtual device_t *clone() const = 0;

    public:
        virtual const initialiser_t &initialiser() const = 0;
        virtual const terminator_t &terminator() const = 0;

        void initialise(const string_t &);

        bool terminating() const;
        string_t terminate();
    };

    class operation_t
    {
    public:
        virtual ~operation_t() = default;
        virtual operation_t *clone() const = 0;

    public:
        virtual bool applicable(const device_t &) const = 0;
        void apply(device_t &) const;

        virtual bool correct_device(const device_t &) const = 0;
        virtual bool intersecting_domain(const operation_t &) const = 0;
        virtual bool intersecting_domain(const terminator_t &) const = 0;
    protected:
        virtual void apply_impl(device_t &) const = 0;
    };

    class noop_t final : public operation_t
    {
    public:
        noop_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;

        bool correct_device(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    private:
        void apply_impl(device_t &) const;
       
    };

    class initialiser_t
    {
    public:
        virtual ~initialiser_t() = default;
        virtual initialiser_t *clone() const = 0;

    public:
        virtual void initialise(device_t &, const string_t &) const = 0;
    };

    class terminator_t
    {
    public:
        virtual ~terminator_t() = default;
        virtual terminator_t *clone() const = 0;

    public:
        virtual bool terminating(const device_t &) const = 0;
        string_t terminate(const device_t &) const;
    protected:
        virtual string_t terminate_impl(const device_t &) const = 0;
    };

    class invalid_operation : public std::exception
    {
    private:
        const device_t &device_;
        const operation_t &operation_;

    public:
        invalid_operation(const device_t &, const operation_t &);

    public:
        const device_t &device() const;
        const operation_t &operation() const;
    };

    class invalid_terminator : public std::exception
    {
    private:
        const terminator_t &terminator_;
        const device_t &device_;

    public:
        invalid_terminator(const terminator_t &, const device_t &);

    public:
        const terminator_t &terminator() const;
        const device_t &device() const;
    };
}

