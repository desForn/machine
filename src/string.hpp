#pragma once
#include "fwd.hpp"

#include <vector>
#include <stdexcept>
#include <compare>

namespace Machine
{
    class alphabet_t;
    class string_t;

    namespace Apparatus
    {
        template<integer_t stride, bool is_const>
        class string_iterator_template_t;
    }

    using string_iterator_t = Apparatus::string_iterator_template_t<1, false>;
    using string_const_iterator_t = Apparatus::string_iterator_template_t<1, true>;
    using string_reverse_iterator_t = Apparatus::string_iterator_template_t<-1, false>;
    using string_const_reverse_iterator_t = Apparatus::string_iterator_template_t<-1, true>;

    class alphabet_t
    {
    private:
        character_t max_character_{0};

    public:
        alphabet_t() noexcept = default;
        alphabet_t(character_t) noexcept;
        character_t max_character() const noexcept;
        bool operator==(const alphabet_t &) const noexcept = default;
        bool operator!=(const alphabet_t &) const noexcept = default;
        void swap(alphabet_t &) noexcept;
    };

    class encoder_t;

    class string_t
    {
        template<index_t stride, bool is_const>
        friend class Apparatus::string_iterator_template_t;

        friend class string_character_proxy_t;
        friend class string_character_const_proxy_t;

    public:
        class string_character_proxy_t
        {
            template<index_t stride, bool is_const>
            friend class Apparatus::string_iterator_template_t;

            friend class string_t;

        private:
            string_t *ptr_{nullptr};
            index_t pos_{0};

        private:
            string_character_proxy_t() noexcept = default;
        public:
            ~string_character_proxy_t() = default;

            string_character_proxy_t(const string_character_proxy_t &) noexcept = default;
            string_character_proxy_t &operator=(
                    const string_character_proxy_t &) noexcept = default;

            string_character_proxy_t(string_character_proxy_t &&) noexcept = default;
            string_character_proxy_t &operator=(string_character_proxy_t &&) noexcept = default;

            string_character_proxy_t(string_t *, index_t) noexcept;

        public:
            operator character_t() const;
            const string_character_proxy_t &operator=(character_t) const;
            string_character_proxy_t &increment(integer_t);
        };

        class string_character_const_proxy_t
        {
            template<index_t stride, bool is_const>
            friend class Apparatus::string_iterator_template_t;

            friend class string_t;

        private:
            const string_t *ptr_{nullptr};
            index_t pos_{0};

        private:
            string_character_const_proxy_t() noexcept = default;
        public:
            ~string_character_const_proxy_t() = default;

            string_character_const_proxy_t(
                    const string_character_const_proxy_t &) noexcept = default;
            string_character_const_proxy_t &operator=(
                    const string_character_const_proxy_t &) noexcept = default;

            string_character_const_proxy_t(string_character_const_proxy_t &&) noexcept = default;
            string_character_const_proxy_t &operator=(
                    string_character_const_proxy_t &&) noexcept = default;

            string_character_const_proxy_t(const string_t *, index_t) noexcept;

        public:
            operator character_t() const;
            string_character_const_proxy_t &increment(integer_t);
        };

        class string_pos_proxy_t
        {
            friend class string_t;

        private:
            string_t *ptr_{nullptr};

        private:
            string_pos_proxy_t() noexcept = default;
        public:
            ~string_pos_proxy_t() = default;

        private:
            string_pos_proxy_t(const string_pos_proxy_t &) noexcept = default;
            string_pos_proxy_t &operator=(const string_pos_proxy_t &) noexcept = default;

            string_pos_proxy_t(string_pos_proxy_t &&) noexcept = default;
            string_pos_proxy_t &operator=(string_pos_proxy_t &&) noexcept = default;

            string_pos_proxy_t(string_t *) noexcept;

        public:
            operator index_t() const;
            string_pos_proxy_t &operator=(index_t);
        };

    public:
        using iterator = string_iterator_t;
        using const_iterator = string_const_iterator_t;
        using reverse_iterator = string_reverse_iterator_t;
        using const_reverse_iterator = string_const_reverse_iterator_t;

    private:
        alphabet_t alphabet_{0};
        std::vector<character_t> string_{};
        index_t pos_{std::size(string_) - 1};

    public:
        string_t() noexcept(noexcept(std::vector<character_t>()));
        string_t(const alphabet_t &) noexcept(noexcept(std::vector<character_t>()));

    public:
        bool empty() const noexcept;
        std::size_t size() const noexcept;
        void resize(index_t, character_t = 0);
        character_t see() const;
        bool see(character_t) const noexcept;
        character_t pop();
        void push(character_t);

        string_pos_proxy_t pos() noexcept;
        index_t pos() const noexcept;
        bool athome() const noexcept;
        bool top() const noexcept;
        void move_l();
        void move_r(character_t);
        void print(character_t);

        string_t &clear() noexcept;

        const alphabet_t &alphabet() const noexcept;

        std::string print_state(const encoder_t &) const;
        std::string print_state_reverse(const encoder_t &) const;

        string_t reverse() const;

        std::strong_ordering operator<=>(const string_t &) const;
        bool operator==(const string_t &) const;
        bool operator!=(const string_t &) const;

        void swap(string_t &) noexcept;

    public:
        string_character_proxy_t operator[](index_t);
        character_t operator[](index_t) const;

        string_iterator_t begin() noexcept;
        string_iterator_t end() noexcept;

        string_const_iterator_t begin() const noexcept;
        string_const_iterator_t end() const noexcept;

        string_const_iterator_t cbegin() noexcept;
        string_const_iterator_t cend() noexcept;

        string_reverse_iterator_t rbegin() noexcept;
        string_reverse_iterator_t rend() noexcept;

        string_const_reverse_iterator_t rbegin() const noexcept;
        string_const_reverse_iterator_t rend() const noexcept;

        string_const_reverse_iterator_t crbegin() noexcept;
        string_const_reverse_iterator_t crend() noexcept;
    };

    void swap(alphabet_t &, alphabet_t &) noexcept;
    void swap(string_t &, string_t &) noexcept;

    namespace Apparatus
    {
        template<integer_t stride, bool is_const>
        class string_iterator_template_t
        {
            static_assert(stride != 0);

            template<integer_t, bool>
            friend class string_iterator_template_t;

        private:
            using value_t = std::conditional_t<is_const, const character_t, character_t>;
            using container_t = std::conditional_t<is_const, const string_t, string_t>;
            using proxy_t = std::conditional_t<is_const, string_t::string_character_const_proxy_t,
                  string_t::string_character_proxy_t>;

        private:
            proxy_t proxy_{};

        public:
            string_iterator_template_t() noexcept = default;
            ~string_iterator_template_t() = default;

            string_iterator_template_t(const string_iterator_template_t &) noexcept = default;
            string_iterator_template_t &operator=(
                    const string_iterator_template_t &) noexcept = default;

            string_iterator_template_t(string_iterator_template_t &&) noexcept = default;
            string_iterator_template_t &operator=(string_iterator_template_t &&) noexcept = default;

            string_iterator_template_t(const string_iterator_template_t<stride, false> &)
                requires(is_const);

            string_iterator_template_t(container_t *, index_t);

        public:
            proxy_t &operator*() noexcept;
            const proxy_t &operator*() const noexcept;
            string_iterator_template_t operator+(integer_t) noexcept;
            string_iterator_template_t operator-(integer_t) noexcept;
            string_iterator_template_t &operator+=(integer_t) noexcept;
            string_iterator_template_t &operator-=(integer_t) noexcept;
            string_iterator_template_t &operator++() noexcept;
            string_iterator_template_t &operator--() noexcept;
            string_iterator_template_t operator++(int) noexcept;
            string_iterator_template_t operator--(int) noexcept;

            std::strong_ordering operator<=>(const string_iterator_template_t &) const;
            bool operator==(const string_iterator_template_t &) const;
            bool operator!=(const string_iterator_template_t &) const;

            void swap(string_iterator_template_t &) noexcept;
        };
        
        template<integer_t stride, bool is_const>
        string_iterator_template_t<stride, is_const>::string_iterator_template_t(
                const string_iterator_template_t<stride, false> &arg) requires(is_const) :
            proxy_{arg.proxy_.ptr_, arg.proxy_.pos_} {}

        template<integer_t stride, bool is_const>
        string_iterator_template_t<stride, is_const>::string_iterator_template_t(
                container_t *ptr, index_t pos) : proxy_{ptr, pos} {}

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator*() noexcept -> proxy_t &
            { return proxy_; }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator*() const noexcept
            -> const proxy_t &
            { return proxy_; }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator+(integer_t arg) noexcept ->
            string_iterator_template_t
        {
            string_iterator_template_t ret{*this};
            ret += arg;
            return ret;
        }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator-(integer_t arg) noexcept ->
            string_iterator_template_t
        {
            string_iterator_template_t ret{*this};
            ret -= arg;
            return ret;
        }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator+=(integer_t arg) noexcept ->
            string_iterator_template_t &
        {
            proxy_.pos_ += arg * stride;
            return *this;
        }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator-=(integer_t arg) noexcept ->
            string_iterator_template_t &
        {
            proxy_.pos_ -= arg * stride;
            return *this;
        }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator++() noexcept ->
            string_iterator_template_t &
            { return *this += 1; }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator--() noexcept ->
            string_iterator_template_t &
            { return *this -= 1; }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator++(int) noexcept ->
            string_iterator_template_t
        {
            string_iterator_template_t ret{*this};
            ++*this;
            return ret;
        }

        template<integer_t stride, bool is_const>
        auto string_iterator_template_t<stride, is_const>::operator--(int) noexcept ->
            string_iterator_template_t
        {
            string_iterator_template_t ret{*this};
            --*this;
            return ret;
        }

        template<integer_t stride, bool is_const>
        std::strong_ordering string_iterator_template_t<stride, is_const>::operator<=>
            (const string_iterator_template_t &arg) const
        {
            if (proxy_.ptr_ != arg.proxy_.ptr_)
                throw std::runtime_error{"In Machine::Apparatus::string_iterator_template_t<stride"
                    ", is_cost>::operator<=>(const string_iterator_template_t &) const:\n"
                    "Comparison between iterators pointing to different strings.\n"};

            if constexpr (stride > 0)
                return proxy_.pos_ <=> arg.proxy_.pos_;
            else
                return arg.proxy_.pos_ <=> proxy_.pos_;
        }

        template<integer_t stride, bool is_const>
        bool string_iterator_template_t<stride, is_const>::operator==
            (const string_iterator_template_t &arg) const
            { return (*this <=> arg) == std::strong_ordering::equivalent; }

        template<integer_t stride, bool is_const>
        bool string_iterator_template_t<stride, is_const>::operator!=
            (const string_iterator_template_t &arg) const
            { return not (*this == arg); }

        template<integer_t stride, bool is_const>
        void string_iterator_template_t<stride, is_const>::swap(string_iterator_template_t &arg)
        noexcept
        {
            auto temp = proxy_;
            proxy_ = arg.proxy_;
            arg.proxy_ = temp;
            return;
        }
    }

    template<index_t stride, bool is_const>
    void swap(Apparatus::string_iterator_template_t<stride, is_const> &arg0,
              Apparatus::string_iterator_template_t<stride, is_const> &arg1) noexcept
        { arg0.swap(arg1); }
}

