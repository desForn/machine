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
    class noop_operation_t;

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
        std::vector<std::string> output_{};
        control_t *first_control_{nullptr};
        machine_state_t state_{machine_state_t::invalid};
        bool deterministic_{true};

    public:
        machine_t() = delete;
        virtual ~machine_t() = default;

        machine_t(const machine_t &) = delete;
        machine_t &operator=(const machine_t &) = delete;

        machine_t(machine_t &&) noexcept = default;
        machine_t &operator=(machine_t &&) noexcept = default;

        machine_t(std::istream &);
        machine_t(std::vector<std::unique_ptr<device_t>>,
                  std::vector<std::shared_ptr<operation_t>>);

    public:
        machine_t &load(std::istream &);
        void store(std::ostream &) const;

    public:
        const std::vector<std::unique_ptr<device_t>> &devices() const noexcept;
        const std::vector<std::shared_ptr<operation_t>> &instruction_set() const noexcept;
        const std::vector<std::string> &output() const noexcept;
        bool deterministic() const noexcept;
        machine_state_t state() const noexcept;

    public:
        void initialise(const std::string &);
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
    protected:
        std::unique_ptr<encoder_t> encoder_;

    public:
        device_t() = delete;
        virtual ~device_t() = default;

        device_t(const device_t &) = delete;
        device_t &operator=(const device_t &) = delete;

        device_t(device_t &&) noexcept = default;
        device_t &operator=(device_t &&) noexcept = default;

        device_t(std::unique_ptr<encoder_t>) noexcept;
        virtual device_t *clone() const = 0;

    public:
        const encoder_t &encoder() const;
        virtual const initialiser_t &initialiser() const = 0;
        virtual const terminator_t &terminator() const = 0;
        virtual std::string print_state() const = 0;

        void initialise(const std::string &);

        bool terminating() const;
        std::string terminate();
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

    class noop_operation_t final : public operation_t
    {
    public:
        noop_operation_t *clone() const override;

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
        virtual void initialise(device_t &, const std::string &) const = 0;
    };

    class terminator_t
    {
    public:
        virtual ~terminator_t() = default;
        virtual terminator_t *clone() const = 0;

    public:
        virtual bool terminating(const device_t &) const = 0;
        virtual std::string terminate(const device_t &) const = 0;
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

