#pragma once
#include "machine.hpp"

#include <list>
#include <thread>
#include <mutex>
#include <map>

#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/dom/elements.hpp"

namespace Machine
{
    class console_t final
    {
    public:
        enum console_state_t : index_t
        {
            empty = 0,
            deterministic = 1,
            program_loaded = 2,
            running = 4
        };

    private:
        class tui_t
        {
        private:
            class devices_t
            {
            private:
                console_t &console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                std::vector<int> dimensions_{6, 5, 6, 5};
                index_t padding_{16};
                float scroll_x_{0};
                float scroll_y_{0};
                ftxui::Component slider_x_;
                ftxui::Component slider_y_;
                std::vector<std::shared_ptr<std::string>> input_strings_;

            public:
                devices_t() = delete;
                ~devices_t() = default;

                devices_t(const devices_t &) = delete;
                devices_t &operator=(const devices_t &) = delete;

                devices_t(devices_t &&) noexcept = delete;
                devices_t &operator=(devices_t &&) noexcept = delete;

                devices_t(console_t &);

            public:
                void update();
                std::vector<std::shared_ptr<std::string>> &input_strings();
                ftxui::Component &component();
            };

            class summary_t
            {
            private:
                console_t &console_;
                ftxui::Component component_;

            public:
                summary_t() = delete;
                ~summary_t() = default;

                summary_t(const summary_t &) = delete;
                summary_t &operator=(const summary_t &) = delete;

                summary_t(summary_t &&) noexcept = delete;
                summary_t &operator=(summary_t &&) noexcept = delete;

                summary_t(console_t &);

            public:
                ftxui::Component &component();
            };

            class applicable_instructions_t
            {
            private:
                struct bool_struct { bool bool_; };

                console_t &console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                float scroll_x_{0};
                float scroll_y_{0};
                ftxui::Component slider_x_;
                ftxui::Component slider_y_;
                std::vector<int> dimensions_;
                index_t padding_{0};
                std::vector<bool_struct> bool_vector_;
                index_t selected_;

            public:
                applicable_instructions_t() = delete;
                ~applicable_instructions_t() = default; 

                applicable_instructions_t(const applicable_instructions_t &) = delete;
                applicable_instructions_t &operator=(const applicable_instructions_t &) = delete;

                applicable_instructions_t(applicable_instructions_t &&) noexcept = delete;
                applicable_instructions_t &operator=(
                        applicable_instructions_t &&) noexcept = delete;

                applicable_instructions_t(console_t &);

            public:
                void update();
                ftxui::Component &component();
            };

            class menu_t
            {
            private:
                const std::vector<std::string> tab_entries_
                    {"Initialise all ", "Initialise individually", "Load program"};

                console_t &console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                std::vector<std::shared_ptr<std::string>> strings_;
                ftxui::ButtonOption button_style_{ftxui::ButtonOption::Simple()};
                ftxui::InputOption input_style_{ftxui::InputOption::Default()};
                int tab_selected_{2};
                float scroll_x_{0};
                float scroll_y_{0};
                ftxui::Component slider_x_;
                ftxui::Component slider_y_;

            public:
                menu_t() = delete;
                ~menu_t() = default;

                menu_t(const menu_t &) = delete;
                menu_t &operator=(const menu_t &) = delete;

                menu_t(menu_t &&) noexcept = delete;
                menu_t &operator=(menu_t &&) noexcept = delete;

                menu_t(console_t &);

            public:
                void update();
                ftxui::Component &component();
                std::vector<std::shared_ptr<std::string>> &strings();
                int &tab_selected();
            };

            class machines_t
            {
            private:
                console_t &console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                std::unique_lock<std::mutex> &lock_{console_.tui().lock_};
                std::array<std::string, 6> strings_;

            public:
                machines_t() = delete;
                ~machines_t() = default;

                machines_t(const machines_t &) = delete;
                machines_t &operator=(const machines_t &) = delete;

                machines_t(machines_t &&) noexcept = delete;
                machines_t &operator=(machines_t &&) noexcept = delete;

                machines_t(console_t &);

            public:
                void update();
                ftxui::Component &component();
            };

            class program_t
            {
            private:
                console_t &console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                float scroll_x_{0};
                float scroll_y_{0};
                ftxui::Component slider_x_;
                ftxui::Component slider_y_;
                std::vector<int> dimensions_;
                index_t padding_{0};

            public:
                program_t() = delete;
                ~program_t() = default;

                program_t(const program_t &) = delete;
                program_t &operator=(const program_t &) = delete;

                program_t(program_t &&) noexcept = delete;
                program_t &operator=(program_t &&) noexcept = delete;

                program_t(console_t &);

            public:
                void update();
                ftxui::Component &component();
            };

        private:
            console_t &console_;
            std::unique_lock<std::mutex> lock_{console_.mutex(), std::defer_lock_t{}};
            devices_t devices_{console_};
            summary_t summary_{console_};
            applicable_instructions_t applicable_instructions_{console_};
            menu_t menu_{console_};
            machines_t machines_{console_};
            program_t program_{console_};
            ftxui::ScreenInteractive screen_{ftxui::ScreenInteractive::Fullscreen()};

        public:
            tui_t() = delete;
            ~tui_t() = default;

            tui_t(const tui_t &) = delete;
            tui_t &operator=(const tui_t &) = delete;

            tui_t(tui_t &&) noexcept = delete;
            tui_t &operator=(tui_t &&) noexcept = delete;

            tui_t(console_t &);

        public:
            devices_t &devices();
            summary_t &summary();
            applicable_instructions_t &applicable_instructions();
            menu_t &menu();
            machines_t &machines();
            program_t &program();
            
        public:
            ftxui::Component component();
            void update_all();
            void loop();
            void exit_loop();
            int width(index_t) const;
            int height(index_t) const;

        public:
            static ftxui::ScreenInteractive &screen();
        };

        using ptr_t = std::shared_ptr<machine_t>;
        using list_t = std::list<ptr_t>;
        using it_t = list_t::iterator;

    private:
        static inline list_t null_list_{nullptr};
        static inline std::atomic<bool> console_constructed_{false};

        const index_t n_threads_;
        std::thread thread_{};
        list_t invalid_machines_{};
        list_t running_machines_{}; 
        std::map<std::vector<std::string>, list_t> halted_machines_{}; 
        list_t blocked_machines_{}; 
        it_t focus_{std::begin(null_list_)}; 
        std::mutex mutex_{}; 
        std::atomic<bool> pause_ = false;
        std::atomic<console_state_t> state_{console_state_t::empty};
        std::atomic<index_t> instruction_counter_{0};
        std::vector<std::shared_ptr<std::string>> strings_{std::make_shared<std::string>(),
            std::make_shared<std::string>()};
        tui_t tui_{*this};

    public:
        console_t();
        ~console_t();

        console_t(const console_t &) = delete;
        console_t &operator=(const console_t &) = delete;

        console_t(console_t &&) noexcept = delete;
        console_t &operator=(console_t &&) noexcept = delete;

        console_t(index_t);

    public:
        console_t &load_program(std::string);
        console_t &initialise(std::string);
        console_t &initialise_all(std::string);
        console_t &initialise_individually(const std::vector<std::string> &);
        console_t &step();
        console_t &run();
        console_t &step_all();
        console_t &run_all();
        console_t &pause();
        console_t &launch_tui();
        console_t &close_tui();

    private:
        void clear() noexcept; // The caller must have locked mutex_
        void reset(); // Idem
        void step(list_t &, it_t, list_t &, list_t &); // The caller must have locked mutex_ and
                                                       // each list must be modifiable without race
                                                       // conditions
        void load_program();
        void initialise_all();
        void initialise_individually();
        void change_focus(machine_t::machine_state_t, index_t); // The caller must lock mutex_
        void change_focus_class(index_t); // Idem
        std::shared_ptr<std::string> program_name();
        std::shared_ptr<std::string> initialiser_string();
        std::shared_ptr<std::string> initialiser_string_vector(index_t);
        it_t focus();
        tui_t &tui();
        std::mutex &mutex();
        std::atomic<index_t> &instruction_counter();
    };
}

