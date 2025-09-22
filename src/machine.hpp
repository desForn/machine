#pragma once

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
    private:
        class program_t
        {
        private:
            std::vector<std::shared_ptr<operation_t>> instruction_set_;
            std::vector<std::vector<std::shared_ptr<operation_t>>::const_iterator> search_table_;
            index_t first_control_;
            index_t computation_bits_;
            bool deterministic_;

        public:
            program_t() = delete;
            ~program_t() = default;

            program_t(const program_t &) = delete;
            program_t &operator=(const program_t &) = delete;

            program_t(program_t &&) noexcept = delete;
            program_t &operator=(program_t &&) noexcept = delete;

            program_t(const std::vector<std::unique_ptr<device_t>> &,
                      std::vector<std::shared_ptr<operation_t>>);

        public:
            template<class self_t>
            decltype(auto) instruction_set(this self_t &&self)
                { return (std::forward<self_t>(self).instruction_set_); }

            template<class self_t>
            decltype(auto) search_table(this self_t &&self)
                { return (std::forward<self_t>(self).search_table_); }

            template<class self_t>
            decltype(auto) first_control(this self_t &&self)
                { return (std::forward<self_t>(self).first_control_); }

            template<class self_t>
            decltype(auto) computation_bits(this self_t &&self)
                { return (std::forward<self_t>(self).computation_bits_); }

            template<class self_t>
            decltype(auto) deterministic(this self_t &&self)
                { return (std::forward<self_t>(self).deterministic_); }
        };

    public:
        enum class machine_state_t { invalid, running, halted, blocked };

    private:
        std::vector<std::unique_ptr<device_t>> devices_;
        std::shared_ptr<program_t> program_;
        std::vector<std::vector<std::shared_ptr<operation_t>>::const_iterator>
            applicable_instructions_{};
        std::vector<std::string> output_;
        std::vector<index_t> computation_{};
        index_t computation_size_{0};
        index_t next_instruction_{negative_1};
        machine_state_t state_{machine_state_t::invalid};

    public:
        machine_t() = delete;
        virtual ~machine_t() = default;

        machine_t(const machine_t &);
        machine_t &operator=(const machine_t &);

        machine_t(machine_t &&) noexcept = default;
        machine_t &operator=(machine_t &&) noexcept = default;

        machine_t(std::vector<std::unique_ptr<device_t>>,
                  std::vector<std::shared_ptr<operation_t>>);

        machine_t(std::ifstream &);

    public:
        void load_program(std::ifstream &);
        void store_program(std::ostream &) const;
        index_t n_applicable_instructions() const;
        std::vector<std::string> print_instruction(index_t) const;
        std::vector<std::string> print_state() const;
        std::string next_instruction(index_t) const;

    public:
        const std::vector<std::unique_ptr<device_t>> &devices() const noexcept;
        const std::vector<std::shared_ptr<operation_t>> &instruction_set() const noexcept;
        const std::vector<std::string> &output() const noexcept;
        bool deterministic() const noexcept;
        machine_state_t state() const noexcept;

    public:
        void initialise(const std::string &);
        void initialise(std::span<const std::string>);
        void next();
        void next(index_t);
        void run();
        std::vector<std::string> print_applicable_instructions() const;
        std::string print_encoder(index_t) const;
        std::string print_initialiser(index_t) const;
        std::string print_terminator(index_t) const;
        void select_instruction(index_t);
        index_t selected_instruction() const;
        index_t computation(index_t) const;
        index_t computation_size() const;
        bool terminating() const;

    private:
        void terminate();
        void applicable_instructions_apparatus();
        void computation_append();
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
        virtual std::string print_name() const = 0;
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

