#include "ram.hpp"

namespace Machine
{
    ram_initialiser_empty_t *ram_initialiser_empty_t::clone() const
        { return new ram_initialiser_empty_t{*this}; }

    void ram_initialiser_empty_t::initialise(device_t &device, const std::string &) const
        { dynamic_cast<ram_t &>(device).clear(); }

    ram_initialiser_string_t *ram_initialiser_string_t::clone() const
        { return new ram_initialiser_string_t{*this}; }

    void ram_initialiser_string_t::initialise(device_t &device, const std::string &string) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);
        const encoder_separator_t &encoder =
            dynamic_cast<const encoder_separator_t &>(ram.encoder());
        const encoder_t &parent_encoder = encoder.parent_encoder();
        char separator = encoder.separator();

        ram.clear();

        index_t c = 0;
        for (auto i = std::cbegin(string); i != std::cend(string); ++c)
        {
            if (*i == separator)
                ++i;

            auto j = i;

            while (j != std::cend(string) and *j != separator)
                ++j;

            ram[c] = unsigned_number_t{std::string_view{i, j}, parent_encoder};

            i = j;
        }

        return;
    }

    ram_terminator_empty_t *ram_terminator_empty_t::clone() const
        { return new ram_terminator_empty_t{*this}; }

    bool ram_terminator_empty_t::terminating(const device_t &) const { return true; }

    std::string ram_terminator_empty_t::terminate(const device_t &) const { return {}; }

    ram_terminator_string_t *ram_terminator_string_t::clone() const
        { return new ram_terminator_string_t{*this}; }

    bool ram_terminator_string_t::terminating(const device_t &) const { return true; }

    std::string ram_terminator_string_t::terminate(const device_t &device) const
    {
        const ram_t &ram = dynamic_cast<const ram_t &>(device);
        const encoder_separator_t &encoder =
            dynamic_cast<const encoder_separator_t &>(ram.encoder());
        const encoder_t &parent_encoder = encoder.parent_encoder();
        char separator = encoder.separator();

        const index_t n = ram.n_registers();

        std::string ret;

        for (index_t i = 0; i != n; ++i)
        {
            ret += static_cast<const unsigned_number_t &>(ram[i]).print(parent_encoder);

            if (i != n - 1)
                ret += separator;
        }

        return ret;
    }

    ram_t::register_proxy_t::register_proxy_t(ram_t &ram, index_t i) :
        i_{i}, registers_{ram.registers_} {}

    ram_t::register_proxy_t &ram_t::register_proxy_t::operator=(unsigned_number_t arg)
    {
        if (i_ >= std::size(registers_))
            registers_.resize(i_ + 1);
        registers_[i_] = std::move(arg);

        return *this;
    }

    ram_t::register_proxy_t::operator unsigned_number_t &()
    {
        if (i_ >= std::size(registers_))
            registers_.resize(i_ + 1);

        return registers_[i_];
    }

    ram_t::register_const_proxy_t::register_const_proxy_t(const ram_t &ram, index_t i) :
        i_{i}, registers_{ram.registers_} {}

    ram_t::register_const_proxy_t::operator const unsigned_number_t &()
    {
        if (i_ >= std::size(registers_))
            return zero_;

        return registers_[i_];
    }

    ram_t::ram_t(const ram_t &arg) : device_t{std::unique_ptr<encoder_t>(arg.encoder_->clone())},
        registers_{arg.registers_}, memory_{arg.memory_} {}

    ram_t::ram_t(std::unique_ptr<encoder_t> encoder, std::unique_ptr<ram_initialiser_t> initialiser,
        std::unique_ptr<ram_terminator_t> terminator) :
        device_t{std::move(encoder)}, initialiser_{std::move(initialiser)},
        terminator_{std::move(terminator)} {}

    ram_t *ram_t::clone() const { return new ram_t{*this}; }

    const ram_initialiser_t &ram_t::initialiser() const { return *initialiser_; }

    const ram_terminator_t &ram_t::terminator() const { return *terminator_; }

    std::string ram_t::print_name() const { return "ram"; }

    std::string ram_t::print_state() const
    {
        if (typeid(encoder()) != typeid(encoder_separator_t))
            return {};

        const encoder_separator_t &encoder =
            dynamic_cast<const encoder_separator_t &>(this->encoder());
        const encoder_t &parent_encoder = encoder.parent_encoder();
        const char separator = encoder.separator();

        const index_t n = n_registers();

        std::string ret;

        for (index_t i = 0; i != n; ++i)
        {
            ret += 'r';
            ret += std::to_string(i);
            ret += separator;
            ret += registers_[i].print(parent_encoder);
            ret += separator;
        }

        for (const auto &i : memory_)
        {
            ret += 'm';
            ret += i.first.print(parent_encoder);
            ret += separator;
            ret += i.second.print(parent_encoder);
            ret += separator;
        }

        if (not std::empty(ret))
            ret.pop_back();

        return ret;
    }

    void ram_t::clear()
    {
        registers_.clear();
        memory_.clear();

        return;
    }

    ram_t::register_proxy_t ram_t::operator[](index_t i) { return {*this, i}; }

    ram_t::register_const_proxy_t ram_t::operator[](index_t i) const { return {*this, i}; }

    std::map<unsigned_number_t, unsigned_number_t> &ram_t::memory() { return memory_; }

    index_t ram_t::n_registers() const
    {
        auto it = std::find_if(std::crbegin(registers_), crend(registers_),
                [](const unsigned_number_t &a) { return not a.zero(); });

        return std::distance(it, std::crend(registers_));
    }

    bool ram_operation_t::correct_device(const device_t &device) const
        { return typeid(device) == typeid(ram_t); }

    bool ram_operation_t::intersecting_domain(const terminator_t &) const { return true; } 

    bool ram_operation_non_test_t::applicable(const device_t &) const { return true; }

    bool ram_operation_non_test_t::intersecting_domain(const operation_t &) const { return true; }

    void ram_operation_test_t::apply(device_t &device) const
    {
        if (not applicable(device))
            throw invalid_operation_t(*this, device);

        return;
    }

    ram_operation_load_t::ram_operation_load_t(index_t reg_destination, index_t reg_source) :
        reg_destination_{reg_destination}, reg_source_{reg_source} {}

    ram_operation_load_t *ram_operation_load_t::clone() const
        { return new ram_operation_load_t{*this}; }

    void ram_operation_load_t::apply(device_t &device) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);
        const ram_t &cram = ram;

        const auto &memory = ram.memory();

        const unsigned_number_t &address = cram[reg_source_];

        auto it = memory.find(address);

        if (it == std::cend(memory))
            ram[reg_destination_] = 0;
        else
            ram[reg_destination_] = it->second;

        return;
    }

    index_t ram_operation_load_t::reg_destination() const { return reg_destination_; }
    index_t ram_operation_load_t::reg_source() const { return reg_source_; }

    ram_operation_store_t::ram_operation_store_t(index_t reg_destination, index_t reg_source) :
        reg_destination_{reg_destination}, reg_source_{reg_source} {}

    ram_operation_store_t *ram_operation_store_t::clone() const
        { return new ram_operation_store_t{*this}; }

    void ram_operation_store_t::apply(device_t &device) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);
        const ram_t &cram = ram;

        auto &memory = ram.memory();

        const unsigned_number_t &address = cram[reg_destination_];
        auto it = memory.find(address);

        if (it == std::cend(memory))
            memory.emplace(address, cram[reg_source_]);
        else
            it->second = cram[reg_source_];

        return;
    }

    index_t ram_operation_store_t::reg_destination() const { return reg_destination_; }
    index_t ram_operation_store_t::reg_source() const { return reg_source_; }

    ram_operation_add_t::ram_operation_add_t(index_t reg_destination, index_t reg_source_0,
        index_t reg_source_1) : reg_destination_{reg_destination}, reg_source_0_{reg_source_0},
        reg_source_1_{reg_source_1} {}

    ram_operation_add_t *ram_operation_add_t::clone() const
        { return new ram_operation_add_t{*this}; }

    void ram_operation_add_t::apply(device_t &device) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);
        const ram_t &cram = ram;

        ram[reg_destination_] = cram[reg_source_0_] + cram[reg_source_1_];

        return;
    }

    index_t ram_operation_add_t::reg_destination() const { return reg_destination_; }
    index_t ram_operation_add_t::reg_source_0() const { return reg_source_0_; }
    index_t ram_operation_add_t::reg_source_1() const { return reg_source_1_; }

    ram_operation_sub_t::ram_operation_sub_t(index_t reg_destination, index_t reg_source_0,
        index_t reg_source_1) : reg_destination_{reg_destination}, reg_source_0_{reg_source_0},
        reg_source_1_{reg_source_1} {}

    ram_operation_sub_t *ram_operation_sub_t::clone() const
        { return new ram_operation_sub_t{*this}; }

    void ram_operation_sub_t::apply(device_t &device) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);
        const ram_t &cram = ram;

        const unsigned_number_t &arg0 = cram[reg_source_0_];
        const unsigned_number_t &arg1 = cram[reg_source_1_];

        if (arg0 <= arg1)
            ram[reg_destination_] = 0;
        else
            ram[reg_destination_] = arg0 - arg1;

        return;
    }

    index_t ram_operation_sub_t::reg_destination() const { return reg_destination_; }
    index_t ram_operation_sub_t::reg_source_0() const { return reg_source_0_; }
    index_t ram_operation_sub_t::reg_source_1() const { return reg_source_1_; }

    ram_operation_halve_t::ram_operation_halve_t(index_t reg_destination, index_t reg_source) :
        reg_destination_{reg_destination}, reg_source_{reg_source} {}

    ram_operation_halve_t *ram_operation_halve_t::clone() const
        { return new ram_operation_halve_t{*this}; }

    void ram_operation_halve_t::apply(device_t &device) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);

        const unsigned_number_t &arg = ram[reg_source_];

        ram[reg_destination_] = arg >> 1;

        return;
    }

    index_t ram_operation_halve_t::reg_destination() const { return reg_destination_; }
    index_t ram_operation_halve_t::reg_source() const { return reg_source_; }

    ram_operation_set_0_t::ram_operation_set_0_t(index_t reg_destination) :
        reg_destination_{reg_destination} {}

    ram_operation_set_0_t *ram_operation_set_0_t::clone() const
        { return new ram_operation_set_0_t{*this}; }

    void ram_operation_set_0_t::apply(device_t &device) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);

        ram[reg_destination_] = 0;

        return;
    }

    index_t ram_operation_set_0_t::reg_destination() const { return reg_destination_; }

    ram_operation_set_1_t::ram_operation_set_1_t(index_t reg_destination) :
        reg_destination_{reg_destination} {}

    ram_operation_set_1_t *ram_operation_set_1_t::clone() const
        { return new ram_operation_set_1_t{*this}; }

    void ram_operation_set_1_t::apply(device_t &device) const
    {
        ram_t &ram = dynamic_cast<ram_t &>(device);

        ram[reg_destination_] = 1;

        return;
    }

    index_t ram_operation_set_1_t::reg_destination() const { return reg_destination_; }

    ram_operation_test_gr_t::ram_operation_test_gr_t(index_t reg_source_0, index_t reg_source_1) :
        reg_source_0_{reg_source_0}, reg_source_1_{reg_source_1}
    {
        if (reg_source_0_ == reg_source_1_)
            throw std::runtime_error{"In Machine::ram_operation_test_gr_t::ram_operation_test_gr_t"
                "(index_t, index_t):\nArguments must differ.\n"};

        return;
    }

    ram_operation_test_gr_t *ram_operation_test_gr_t::clone() const
        { return new ram_operation_test_gr_t{*this}; }

    bool ram_operation_test_gr_t::applicable(const device_t &device) const
    {
        const ram_t &ram = dynamic_cast<const ram_t &>(device);

        return static_cast<const unsigned_number_t &>(ram[reg_source_0_]) >
            static_cast<const unsigned_number_t &>(ram[reg_source_1_]);
    }

    bool ram_operation_test_gr_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(ram_operation_test_gr_t))
        {
            const ram_operation_test_gr_t &op =
                dynamic_cast<const ram_operation_test_gr_t &>(operation);

            return reg_source_0_ != op.reg_source_1_ or reg_source_1_ != op.reg_source_0_;
        }

        if (typeid(operation) == typeid(ram_operation_test_eq_t))
        {
            const ram_operation_test_eq_t &op =
                dynamic_cast<const ram_operation_test_eq_t &>(operation);

            return (reg_source_0_ != op.reg_source_0() or reg_source_1_ != op.reg_source_1()) and
                   (reg_source_0_ != op.reg_source_1() or reg_source_1_ != op.reg_source_0());
        }
        
        return true;
    }

    index_t ram_operation_test_gr_t::reg_source_0() const { return reg_source_0_; }
    index_t ram_operation_test_gr_t::reg_source_1() const { return reg_source_1_; }

    ram_operation_test_eq_t::ram_operation_test_eq_t(index_t reg_source_0, index_t reg_source_1) :
        reg_source_0_{reg_source_0}, reg_source_1_{reg_source_1}
    {
        if (reg_source_0_ == reg_source_1_)
            throw std::runtime_error{"In Machine::ram_operation_test_eq_t::ram_operation_test_eq_t"
                "(index_t, index_t):\nArguments must differ.\n"};

        return;
    }

    ram_operation_test_eq_t *ram_operation_test_eq_t::clone() const
        { return new ram_operation_test_eq_t{*this}; }

    bool ram_operation_test_eq_t::applicable(const device_t &device) const
    {
        const ram_t &ram = dynamic_cast<const ram_t &>(device);

        return static_cast<const unsigned_number_t &>(ram[reg_source_0_]) ==
            static_cast<const unsigned_number_t &>(ram[reg_source_1_]);
    }

    bool ram_operation_test_eq_t::intersecting_domain(const operation_t &operation) const
    {
        if (typeid(operation) == typeid(ram_operation_test_gr_t))
        {
            const ram_operation_test_gr_t &op =
                dynamic_cast<const ram_operation_test_gr_t &>(operation);

            return (reg_source_0_ != op.reg_source_0() or reg_source_1_ != op.reg_source_1()) and
                (reg_source_0_ != op.reg_source_1() or reg_source_1_ != op.reg_source_0());
        }

        return true;
    }

    index_t ram_operation_test_eq_t::reg_source_0() const { return reg_source_0_; }
    index_t ram_operation_test_eq_t::reg_source_1() const { return reg_source_1_; }

    ram_operation_test_odd_t::ram_operation_test_odd_t(index_t reg_source) :
        reg_source_{reg_source} {}

    ram_operation_test_odd_t *ram_operation_test_odd_t::clone() const
        { return new ram_operation_test_odd_t{*this}; }

    bool ram_operation_test_odd_t::applicable(const device_t &device) const
        { return dynamic_cast<const ram_t &>(device)[reg_source_] % 2 == 1; }

    bool ram_operation_test_odd_t::intersecting_domain(const operation_t &operation) const
    {
        return typeid(operation) == typeid(ram_operation_test_even_t) and
            dynamic_cast<const ram_operation_test_even_t &>(operation).reg_source() != reg_source_;
    }

    index_t ram_operation_test_odd_t::reg_source() const { return reg_source_; }

    ram_operation_test_even_t::ram_operation_test_even_t(index_t reg_source) :
        reg_source_{reg_source} {}

    ram_operation_test_even_t *ram_operation_test_even_t::clone() const
        { return new ram_operation_test_even_t{*this}; }

    bool ram_operation_test_even_t::applicable(const device_t &device) const
        { return dynamic_cast<const ram_t &>(device)[reg_source_] % 2 == 0; }

    bool ram_operation_test_even_t::intersecting_domain(const operation_t &operation) const
    {
        return typeid(operation) == typeid(ram_operation_test_odd_t) and
            dynamic_cast<const ram_operation_test_odd_t &>(operation).reg_source() != reg_source_;
    }

    index_t ram_operation_test_even_t::reg_source() const { return reg_source_; }
}

