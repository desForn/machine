#pragma once
#include <memory>
#include <span>
#include <iostream>
#include <fstream>

#include "encoder.hpp"

namespace Machine
{
    class machine_t;
    class device_t;
    class initialiser_t;
    class terminator_t;
    class operation_t;
    class noop_t;

    class invalid_operation_t;
    class invalid_terminator_t;

    class control_t;

    class machine_t
    {
    public:
        enum class machine_state_t
            { invalid, running, non_deterministic_decision, blocked, halted };

    private:
        std::vector<std::unique_ptr<device_t>> devices_;
        std::vector<std::shared_ptr<operation_t>> instruction_set_;
        std::vector<std::shared_ptr<operation_t>> applicable_instructions_{};
        std::vector<std::vector<std::shared_ptr<operation_t>>::iterator> search_table_{};
        std::vector<string_t> output_{};
        std::unique_ptr<encoder_t> encoder_;
        control_t *first_control_{nullptr};
        machine_state_t state_{machine_state_t::invalid};
        bool deterministic_ = true;

    public:
        machine_t() = delete;
        virtual ~machine_t() = default;

        machine_t(const machine_t &) = delete;
        machine_t &operator=(const machine_t &) = delete;

        machine_t(machine_t &&) noexcept = default;
        machine_t &operator=(machine_t &&) noexcept = default;

        machine_t(std::istream &);
        machine_t(std::vector<std::unique_ptr<device_t>>,
                  std::vector<std::shared_ptr<operation_t>>,
                  const encoder_t &encoder = encoder_alphabetical_t{26});
        machine_t(std::vector<std::unique_ptr<device_t>>,
                  std::vector<std::shared_ptr<operation_t>>,
                  std::unique_ptr<encoder_t>);

    public:
        machine_t &load(std::istream &);
        void store(std::ostream &) const;

    public:
        decltype(auto) devices(this auto &&self)
            { return std::span(std::begin(self.devices_), std::end(self.devices_)); };

        decltype(auto) instruction_set(this auto &&self)
            { return std::span(std::begin(self.instruction_set_),
                               std::end(self.instruction_set_)); };

        const encoder_t &encoder() const;
        const alphabet_t &alphabet() const;
        std::span<const string_t> output() const;
        bool deterministic() const;
        machine_state_t state() const;

    public:
        void initialise(const std::string &);
        void initialise(const string_t &);
        void next();
        void next(index_t);
        void run();
        std::vector<std::span<const std::shared_ptr<operation_t>>> applicable_instructions() const;
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
        virtual void apply(device_t &) const = 0;

        virtual bool correct_device(const device_t &) const = 0;
        virtual bool intersecting_domain(const operation_t &) const = 0;
        virtual bool intersecting_domain(const terminator_t &) const = 0;
    };

    class noop_t final : public operation_t
    {
    public:
        noop_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        void apply(device_t &) const override;

        bool correct_device(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
       
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
        virtual string_t terminate(const device_t &) const = 0;
    };

    class invalid_operation_t : public std::exception
    {
    private:
        const operation_t &operation_;
        const device_t &device_;

    public:
        invalid_operation_t(const operation_t &, const device_t &);

    public:
        const operation_t &operation() const;
        const device_t &device() const;
    };

    class invalid_terminator_t : public std::exception
    {
    private:
        const terminator_t &terminator_;
        const device_t &device_;

    public:
        invalid_terminator_t(const terminator_t &, const device_t &);

    public:
        const terminator_t &terminator() const;
        const device_t &device() const;
    };
}

