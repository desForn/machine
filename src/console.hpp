#pragma once
#include "machine.hpp"
#include <deque>
#include <atomic>

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
            initialised = 4,
            running = 8,
            stopped = 16,
        };

        class tui_t
        {
        public:
            class devices_t
            {
            private:
                console_t *console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                std::vector<std::shared_ptr<int>> dimensions_{
                    std::make_shared<int>(6), std::make_shared<int>(6), std::make_shared<int>(6),
                    std::make_shared<int>(6)};
                std::shared_ptr<index_t> padding_{std::make_shared<index_t>(0)};
                std::shared_ptr<float> scroll_x_{std::make_shared<float>(0)};
                std::shared_ptr<float> scroll_y_{std::make_shared<float>(0)};
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

                devices_t(console_t *);

            public:
                void update();
                std::vector<std::shared_ptr<std::string>> &input_strings();
                ftxui::Component &component();
            };

            class summary_t
            {
            private:
                ftxui::Component component_;
                console_t *console_;

            public:
                summary_t() = delete;
                ~summary_t() = default;

                summary_t(const summary_t &) = delete;
                summary_t &operator=(const summary_t &) = delete;

                summary_t(summary_t &&) noexcept = delete;
                summary_t &operator=(summary_t &&) noexcept = delete;

                summary_t(console_t *);

            public:
                ftxui::Component &component();
            };

            class applicable_instructions_t
            {
            private:
                struct bool_struct { bool bool_; };

                console_t *console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                std::shared_ptr<float> scroll_x_{std::make_shared<float>(0)};
                std::shared_ptr<float> scroll_y_{std::make_shared<float>(0)};
                ftxui::Component slider_x_;
                ftxui::Component slider_y_;
                std::vector<std::shared_ptr<int>> dimensions_;
                std::shared_ptr<index_t> padding_{std::make_shared<index_t>(0)};
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

                applicable_instructions_t(console_t *);

            public:
                void update();
                ftxui::Component &component();
            };

            class menu_t
            {
            private:
                const std::vector<std::string> tab_entries_
                    {"Initialise all ", "Initialise individually", "Load program"};

                console_t *console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                std::vector<std::shared_ptr<std::string>> strings_;
                ftxui::ButtonOption button_style_{ftxui::ButtonOption::Simple()};
                ftxui::InputOption input_style_{ftxui::InputOption::Default()};
                std::shared_ptr<int> tab_selected_{std::make_shared<int>(0)};
                std::shared_ptr<float> scroll_x_{std::make_shared<float>(0)};
                std::shared_ptr<float> scroll_y_{std::make_shared<float>(0)};
                ftxui::Component slider_x_;
                ftxui::Component slider_y_;

            public:
                menu_t() = delete;
                ~menu_t() = default;

                menu_t(const menu_t &) = delete;
                menu_t &operator=(const menu_t &) = delete;

                menu_t(menu_t &&) noexcept = delete;
                menu_t &operator=(menu_t &&) noexcept = delete;

                menu_t(console_t *);

            public:
                void update();
                ftxui::Component &component();
                std::vector<std::shared_ptr<std::string>> &strings();
            };

            class machines_t
            {
            private:
                console_t *console_;
                volatile bool update_ = true;
                ftxui::Component component_;

            public:
                machines_t() = delete;
                ~machines_t() = default;

                machines_t(const machines_t &) = delete;
                machines_t &operator=(const machines_t &) = delete;

                machines_t(machines_t &&) noexcept = delete;
                machines_t &operator=(machines_t &&) noexcept = delete;

                machines_t(console_t *);

            public:
                void update();
                ftxui::Component &component();
            };

            class program_t
            {
            private:
                console_t *console_;
                volatile bool update_ = true;
                ftxui::Component component_;
                ftxui::Component child_component_{ftxui::Renderer([]{ return ftxui::text(""); })};
                std::shared_ptr<float> scroll_x_{std::make_shared<float>(0)};
                std::shared_ptr<float> scroll_y_{std::make_shared<float>(0)};
                ftxui::Component slider_x_;
                ftxui::Component slider_y_;
                std::vector<std::shared_ptr<int>> dimensions_;
                index_t padding_{0};

            public:
                program_t() = delete;
                ~program_t() = default;

                program_t(const program_t &) = delete;
                program_t &operator=(const program_t &) = delete;

                program_t(program_t &&) noexcept = delete;
                program_t &operator=(program_t &&) noexcept = delete;

                program_t(console_t *);

            public:
                void update();
                ftxui::Component &component();
            };

        private:
            static ftxui::ScreenInteractive screen_;

            console_t *console_;
            devices_t devices_{console_};
            summary_t summary_{console_};
            applicable_instructions_t applicable_instructions_{console_};
            menu_t menu_{console_};
            machines_t machines_{console_};
            program_t program_{console_};

        public:
            tui_t() = delete;
            ~tui_t() = default;

            tui_t(const tui_t &) = delete;
            tui_t &operator=(const tui_t &) = delete;

            tui_t(tui_t &&) noexcept = delete;
            tui_t &operator=(tui_t &&) noexcept = delete;

            tui_t(console_t *);

        public:
            devices_t &devices();
            summary_t &summary();
            applicable_instructions_t &applicable_instructions();
            menu_t &menu();
            machines_t &machines();
            program_t &program();
            
        public:
            void update_all();
            void update();
            void relaunch();
            ftxui::Component component();
            void loop();
            void exit_loop();
            int width(index_t) const;
            int height(index_t) const;

        public:
            static ftxui::ScreenInteractive &screen();
        };

    private:
        std::vector<std::shared_ptr<machine_t>> machines_{};
        std::shared_ptr<machine_t> focus_{nullptr};
        console_state_t state_{console_state_t::empty};
        std::vector<std::shared_ptr<std::string>> strings_{std::make_shared<std::string>(),
            std::make_shared<std::string>()};
        std::atomic<index_t> instruction_counter_{0};
        tui_t tui_{this};

    public:
        console_t() = default;
        ~console_t() = default;

        console_t(const console_t &) = delete;
        console_t &operator=(const console_t &) = delete;

        console_t(console_t &&) noexcept = delete;
        console_t &operator=(console_t &&) noexcept = delete;

    public:
        void load_program();
        void initialise_all();
        void initialise_individually();
        void step();
        void run();
        std::shared_ptr<std::string> program_name();
        std::shared_ptr<std::string> initialiser_string();
        std::shared_ptr<std::string> initialiser_string_vector(index_t);

    private:
        void reset();

    public:
        std::shared_ptr<machine_t> focus();
        tui_t &tui();
        void tui_loop();
    };
}

