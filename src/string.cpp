#include "string.hpp"
#include "encoder.hpp"

#include <algorithm>

namespace Machine
{
    alphabet_t::alphabet_t(character_t max_character) noexcept : max_character_{max_character} {}

    character_t alphabet_t::max_character() const noexcept { return max_character_; }

    void alphabet_t::swap(alphabet_t &arg) noexcept
        { std::swap(max_character_, arg.max_character_); }

    string_t::string_character_proxy_t::string_character_proxy_t(string_t *ptr, index_t pos)
        noexcept : ptr_{ptr}, pos_{pos} {}

    string_t::string_character_proxy_t::operator character_t() const
    {
        if (not ptr_ or pos_ >= ptr_->size())
            throw std::runtime_error{"In Machine::string_t::string_character_proxy_t::"
                "operator character_t():\nOut of bounds.\n"};

        return ptr_->string_[pos_];
    }

    const string_t::string_character_proxy_t &string_t::string_character_proxy_t::operator=(
            character_t c) const
    {
        if (not ptr_ or pos_ >= ptr_->size())
            throw std::runtime_error{"In Machine::string_t::string_character_proxy_t::"
                "operator=(character_t):\nOut of bounds.\n"};

        if (c > ptr_->alphabet().max_character())
            throw std::runtime_error{"In Machine::string_t::string_character_proxy_t::"
                "operator=(character_t):\nInvalid character\n"};

        ptr_->string_[pos_] = c;

        return *this;
    }

    string_t::string_character_proxy_t &string_t::string_character_proxy_t::increment(integer_t arg)
        { pos_ += arg; return *this; }

    string_t::string_character_const_proxy_t::string_character_const_proxy_t(
            const string_t *ptr, index_t pos) noexcept : ptr_{ptr}, pos_{pos} {}

    string_t::string_character_const_proxy_t::operator character_t() const
    {
        if (not ptr_ or pos_ >= ptr_->size())
            throw std::runtime_error{"In Machine::string_t::string_character_const_proxy_t::"
                "operator character_t():\nOut of bounds.\n"};

        return ptr_->string_[pos_];
    }

    string_t::string_character_const_proxy_t &string_t::string_character_const_proxy_t::increment(
            integer_t arg)
        { pos_ += arg; return *this; }

    string_t::string_pos_proxy_t::string_pos_proxy_t(string_t *ptr) noexcept : ptr_{ptr} {}

    string_t::string_pos_proxy_t::operator index_t() const { return ptr_->pos_; }

    string_t::string_pos_proxy_t &string_t::string_pos_proxy_t::operator=(index_t pos)
    {
        if (not ((pos == negative_1 and ptr_->empty()) or pos < ptr_->size()))
            throw std::runtime_error{"In Machine::string_t::string_pos_proxy_t::operator=(index_t):"
                "\nString overflow.\n"};
        ptr_->pos_ = pos;

        return *this;
    }

    string_t::string_t() noexcept(noexcept(std::vector<character_t>())) {}

    string_t::string_t(const alphabet_t &alphabet) noexcept(noexcept(std::vector<character_t>())) :
        alphabet_{alphabet} {}

    bool string_t::empty() const noexcept { return pos_ == negative_1; }

    std::size_t string_t::size() const noexcept { return std::size(string_); }

    void string_t::resize(index_t new_size, character_t c)
    {
        string_.resize(new_size, c);
        if (pos_ >= new_size)
            pos_ = new_size - 1;
        return;
    }

    character_t string_t::see() const
    {
        if (pos_ == negative_1)
            throw std::runtime_error{"In Machine::string_t::see() const:\n"
                "Read past end of file.\n"};
        return string_[pos_];
    }

    bool string_t::see(character_t c) const noexcept
        { return pos_ != negative_1 and string_[pos_] == c; }

    character_t string_t::pop()
    {
        if (pos_ == negative_1)
            throw std::runtime_error{"In Machine::string_t::pop() const:\n"
                "Read past end of file.\n"};

        if (pos_ != std::size(string_) - 1)
            throw std::runtime_error{"In Machine::string_t::pop() const:\n"
                "Can pop only if pos is set to the last element of the string.\n"};

        character_t ret = string_[pos_--];
        string_.pop_back();
        return ret;
    }

    void string_t::push(character_t c)
    {
        if (c > alphabet_.max_character())
            throw std::runtime_error{"In Machine::string_t::push(character_t):\n"
                "Argument '" + std::to_string(c) + "' is larger than the maximum allowed by the " +
                "alphabet '" + std::to_string(alphabet().max_character()) + "'\n."};

        if (pos_ != std::size(string_) - 1)
            throw std::runtime_error{"In Machine::string_t::push(character_t):\n"
                "Can push only if pos is set to the last element of the string."};
        ++pos_;
        string_.push_back(c);
        return;
    }

    string_t::string_pos_proxy_t string_t::pos() noexcept { return {this}; }

    index_t string_t::pos() const noexcept { return pos_; }

    bool string_t::athome() const noexcept { return pos_ == 0; }

    bool string_t::top() const noexcept { return pos_ == std::size(string_) - 1; }

    void string_t::move_l()
    {
        if (pos_ == negative_1 or pos_ == 0)
            throw std::runtime_error{"In Machine::string_t::move_l() const:\n"
                "Read past end of file.\n"};
        --pos_;
        return;
    }

    void string_t::move_r(character_t c)
    {
        if (c > alphabet_.max_character())
            throw std::runtime_error{"In Machine::string_t::move_r(character_t):\n"
                "Argument '" + std::to_string(c) + "' is larger than the maximum allowed by the " +
                "alphabet '" + std::to_string(alphabet().max_character()) + "'\n."};

        if (pos_ == std::size(string_) - 1)
            string_.push_back(c);
        ++pos_;
        return;
    }

    void string_t::print(character_t c)
    {
        if (c > alphabet_.max_character())
            throw std::runtime_error{"In Machine::string_t::print(character_t):\n"
                "Argument '" + std::to_string(c) + "' is larger than the maximum allowed by the " +
                "alphabet '" + std::to_string(alphabet().max_character()) + "'\n."};

        if (pos_ == negative_1)
            throw std::runtime_error{"In Machine::string_t::print(character_t) const:\n"
                "Print past end of file.\n"};

        string_[pos_] = c;

        return;
    }

    string_t &string_t::clear() noexcept { string_.clear(); pos_ = negative_1; return *this; }

    const alphabet_t &string_t::alphabet() const noexcept { return alphabet_; }

    std::string string_t::print_state(const encoder_t &encoder) const
    {
        std::string ret{};

        for (auto i = std::cbegin(string_); i != std::cend(string_); ++i)
            ret.push_back(encoder(*i));

        return ret;
    }

    std::string string_t::print_state_reverse(const encoder_t &encoder) const
    {
        std::string ret{};

        for (auto i = std::crbegin(string_); i != std::crend(string_); ++i)
            ret.push_back(encoder(*i));

        return ret;
    }

    string_t string_t::reverse() const
    {
        string_t ret{alphabet_};

        for (auto it = std::crbegin(*this); it != std::crend(*this); ++it)
            ret.push(*it);

        if (pos_ != negative_1)
            ret.pos_ = std::size(string_) - 1 - pos_;

        return ret;
    }

    std::strong_ordering string_t::operator<=>(const string_t &arg) const
    {
        if (alphabet_ != arg.alphabet_)
            throw std::runtime_error{"In Machine::string_t::operator<=>(const string_t &) const:"
            "\nThe strings have different alphabet.\n"};


        for(auto i = std::crbegin(string_), j = std::crbegin(arg.string_); ;++i, ++j)
        {
            if (i == std::crend(string_))
            {
                if (j == std::crend(arg.string_))
                    return std::strong_ordering::equivalent;
                return std::strong_ordering::less;
            }
            if (j == std::crend(arg.string_))
                return std::strong_ordering::greater;

            if (*i < *j)
                return std::strong_ordering::less;
            if (*j < *i)
                return std::strong_ordering::greater;
        }
    }

    bool string_t::operator==(const string_t &arg) const
        { return (*this <=> arg) == std::strong_ordering::equivalent; }

    bool string_t::operator!=(const string_t &arg) const { return not (*this == arg); }

    void string_t::swap(string_t &arg) noexcept
    {
        using std::swap;

        swap(alphabet_, arg.alphabet_);
        swap(string_, arg.string_);
        swap(pos_, arg.pos_);

        return;
    }

    string_t::string_character_proxy_t string_t::operator[](index_t arg) { return {this, arg}; }

    character_t string_t::operator[](index_t arg) const
    {
        if (arg >= std::size(string_))
            throw std::runtime_error{"In Machine::string_t::operator[](index_t) const:\n"
                "Accessing an empty string.\n"};

        return string_[arg];
    }

    string_iterator_t string_t::begin() noexcept
        { return {this, 0}; }
    string_iterator_t string_t::end() noexcept
        { return {this, size()}; }

    string_const_iterator_t string_t::begin() const noexcept
        { return {this, 0}; }
    string_const_iterator_t string_t::end() const noexcept
        { return {this, size()}; }

    string_const_iterator_t string_t::cbegin() noexcept
        { return {this, 0}; }
    string_const_iterator_t string_t::cend() noexcept
        { return {this, size()}; }

    string_reverse_iterator_t string_t::rbegin() noexcept
        { return {this, size() - 1}; }
    string_reverse_iterator_t string_t::rend() noexcept
        { return {this, negative_1}; }

    string_const_reverse_iterator_t string_t::rbegin() const noexcept
        { return {this, size() - 1}; }
    string_const_reverse_iterator_t string_t::rend() const noexcept
        { return {this, negative_1}; }

    string_const_reverse_iterator_t string_t::crbegin() noexcept
        { return {this, size() - 1}; }
    string_const_reverse_iterator_t string_t::crend() noexcept
        { return {this, negative_1}; }

    void swap(alphabet_t &arg0, alphabet_t &arg1) noexcept { arg0.swap(arg1); }

    void swap(string_t &arg0, string_t &arg1) noexcept { arg0.swap(arg1); }
}

