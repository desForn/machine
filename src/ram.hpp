#pragma once
#include "machine.hpp"
#include "number.hpp"

#include <map>

namespace Machine
{
    class ram_t;
    class ram_operation_t;
    class ram_operation_load_t;
    class ram_operation_store_t;
    class ram_operation_add_t;
    class ram_operation_sub_t;
    class ram_operation_halve_t;
    class ram_operation_set_0_t;
    class ram_operation_set_1_t;
    class ram_operation_test_gr_t;
    class ram_operation_test_eq_t;
    class ram_operation_test_odd_t;
    class ram_operation_test_even_t;
    class ram_initiliaser_t;
    class ram_initialiser_empty_t;
    class ram_initialiser_string_t;
    class ram_terminator_t;
    class ram_terminator_empty_t;
    class ram_terminator_string_t;

    class ram_initialiser_t : public initialiser_t
    {
    public:
        virtual ~ram_initialiser_t() = default;
        ram_initialiser_t *clone() const override = 0;
    };

    class ram_initialiser_empty_t final : public ram_initialiser_t
    {
    public:
        ram_initialiser_empty_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class ram_initialiser_string_t final : public ram_initialiser_t
    {
    public:
        ram_initialiser_string_t *clone() const override;

    public:
        void initialise(device_t &, const std::string &) const override;
    };

    class ram_terminator_t : public terminator_t
    {
    public:
        virtual ~ram_terminator_t() = default;
        ram_terminator_t *clone() const override = 0;
    };

    class ram_terminator_empty_t final : public ram_terminator_t
    {
    public:
        ram_terminator_empty_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class ram_terminator_string_t final : public ram_terminator_t
    {
    public:
        ram_terminator_string_t *clone() const override;

    public:
        bool terminating(const device_t &) const override;
        std::string terminate(const device_t &) const override;
    };

    class ram_t final : public device_t
    {
    private:
        class register_proxy_t
        {
        private:
            const index_t i_;
            std::vector<unsigned_number_t> &registers_;

        public:
            register_proxy_t() = delete;
            ~register_proxy_t() = default;

            register_proxy_t(const register_proxy_t &) = delete;
            register_proxy_t &operator=(const register_proxy_t &) = delete;

            register_proxy_t(register_proxy_t &&) noexcept = delete;
            register_proxy_t &operator=(register_proxy_t &&) noexcept = delete;

            register_proxy_t(ram_t &, index_t);

            register_proxy_t &operator=(unsigned_number_t);

            operator unsigned_number_t &();
        };

        class register_const_proxy_t
        {
        private:
            const index_t i_;
            const std::vector<unsigned_number_t> &registers_;

        public:
            register_const_proxy_t() = delete;
            ~register_const_proxy_t() = default;

            register_const_proxy_t(const register_const_proxy_t &) = delete;
            register_const_proxy_t &operator=(const register_const_proxy_t &) = delete;

            register_const_proxy_t(register_const_proxy_t &&) noexcept = delete;
            register_const_proxy_t &operator=(register_const_proxy_t &&) noexcept = delete;

            register_const_proxy_t(const ram_t &, index_t);

            operator const unsigned_number_t &();
        };

    private:
        static const inline unsigned_number_t zero_{0};

        std::unique_ptr<ram_initialiser_t> initialiser_;
        std::unique_ptr<ram_terminator_t> terminator_;
        std::vector<unsigned_number_t> registers_{};
        std::map<unsigned_number_t, unsigned_number_t> memory_{};

    public:
        ram_t() = delete;
        ~ram_t() = default;

        ram_t(const ram_t &);
        ram_t &operator=(const ram_t &);

        ram_t(ram_t &&) noexcept = default;
        ram_t &operator=(ram_t &&) noexcept = default;

        ram_t(std::unique_ptr<encoder_t>, std::unique_ptr<ram_initialiser_t>,
            std::unique_ptr<ram_terminator_t>);
        ram_t *clone() const override;

    public:
        const ram_initialiser_t &initialiser() const override;
        const ram_terminator_t &terminator() const override;
        std::string print_name() const override;
        std::string print_state() const override;

        void clear();
        register_proxy_t operator[](index_t);
        register_const_proxy_t operator[](index_t) const;
        std::map<unsigned_number_t, unsigned_number_t> &memory();
        index_t n_registers() const;
    };

    class ram_operation_t : public operation_t
    {
    public:
        virtual ~ram_operation_t() = default;

    public:
        bool correct_device(const device_t &) const override;
        bool intersecting_domain(const terminator_t &) const override;
    };

    class ram_operation_non_test_t : public ram_operation_t
    {
    public:
        virtual ~ram_operation_non_test_t() = default;

    public:
        bool applicable(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;
    };

    class ram_operation_test_t : public ram_operation_t
    {
    public:
        virtual ~ram_operation_test_t() = default;

    public:
        void apply(device_t &) const override;
    };

    class ram_operation_load_t final : public ram_operation_non_test_t
    {
    private:
        index_t reg_destination_;
        index_t reg_source_;

    public:
        ram_operation_load_t(index_t, index_t);
        ram_operation_load_t *clone() const override;

    public:
        void apply(device_t &) const override;

        index_t reg_destination() const;
        index_t reg_source() const;
    };

    class ram_operation_store_t final : public ram_operation_non_test_t
    {
    private:
        index_t reg_destination_;
        index_t reg_source_;

    public:
        ram_operation_store_t(index_t, index_t);
        ram_operation_store_t *clone() const override;

    public:
        void apply(device_t &) const override;

        index_t reg_destination() const;
        index_t reg_source() const;
    };

    class ram_operation_add_t final : public ram_operation_non_test_t
    {
    private:
        index_t reg_destination_;
        index_t reg_source_0_;
        index_t reg_source_1_;

    public:
        ram_operation_add_t(index_t, index_t, index_t);
        ram_operation_add_t *clone() const override;

    public:
        void apply(device_t &) const override;

        index_t reg_destination() const;
        index_t reg_source_0() const;
        index_t reg_source_1() const;
    };

    class ram_operation_sub_t final : public ram_operation_non_test_t
    {
    private:
        index_t reg_destination_;
        index_t reg_source_0_;
        index_t reg_source_1_;

    public:
        ram_operation_sub_t(index_t, index_t, index_t);
        ram_operation_sub_t *clone() const override;

    public:
        void apply(device_t &) const override;

        index_t reg_destination() const;
        index_t reg_source_0() const;
        index_t reg_source_1() const;
    };

    class ram_operation_halve_t final : public ram_operation_non_test_t
    {
    private:
        index_t reg_destination_;
        index_t reg_source_;

    public:
        ram_operation_halve_t(index_t, index_t);
        ram_operation_halve_t *clone() const override;

    public:
        void apply(device_t &) const override;

        index_t reg_destination() const;
        index_t reg_source() const;
    };

    class ram_operation_set_0_t final : public ram_operation_non_test_t
    {
    private:
        index_t reg_destination_;

    public:
        ram_operation_set_0_t(index_t);
        ram_operation_set_0_t *clone() const override;

    public:
        void apply(device_t &) const override;

        index_t reg_destination() const;
    };

    class ram_operation_set_1_t final : public ram_operation_non_test_t
    {
    private:
        index_t reg_destination_;

    public:
        ram_operation_set_1_t(index_t);
        ram_operation_set_1_t *clone() const override;

    public:
        void apply(device_t &) const override;

        index_t reg_destination() const;
    };

    class ram_operation_test_gr_t final : public ram_operation_test_t
    {
    private:
        index_t reg_source_0_;
        index_t reg_source_1_;

    public:
        ram_operation_test_gr_t(index_t, index_t);
        ram_operation_test_gr_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;

        index_t reg_source_0() const;
        index_t reg_source_1() const;
    };

    class ram_operation_test_eq_t final : public ram_operation_test_t
    {
    private:
        index_t reg_source_0_;
        index_t reg_source_1_;

    public:
        ram_operation_test_eq_t(index_t, index_t);
        ram_operation_test_eq_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;

        index_t reg_source_0() const;
        index_t reg_source_1() const;
    };

    class ram_operation_test_odd_t final : public ram_operation_test_t
    {
    private:
        index_t reg_source_;

    public:
        ram_operation_test_odd_t(index_t);
        ram_operation_test_odd_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;

        index_t reg_source() const;
    };

    class ram_operation_test_even_t final : public ram_operation_test_t
    {
    private:
        index_t reg_source_;

    public:
        ram_operation_test_even_t(index_t);
        ram_operation_test_even_t *clone() const override;

    public:
        bool applicable(const device_t &) const override;
        bool intersecting_domain(const operation_t &) const override;

        index_t reg_source() const;
    };
}

