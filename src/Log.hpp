
#ifndef Log_hpp
#define Log_hpp

#include <memory>
#include "imgui.h"

namespace eeng {

    /// @brief 
    /// @param fmt 
    /// @param  
    void Log(const char* fmt, ...);

    /// @brief 
    /// @param label 
    /// @param p_open 
    void LogDraw(const char* label, bool* p_open = nullptr);

    /// @brief 
    void LogClear();

    namespace internal {

        /// @brief ImGui log widget
        ///  Adapted from imgui_demo.cpp
        struct ImGuiLogWidget
        {
            ImGuiTextBuffer Buf;
            ImGuiTextFilter Filter;
            ImVector<int> LineOffsets;
            bool AutoScroll;
            bool ScrollToBottom;

            ImGuiLogWidget();

            void Clear();

            void AddLog(const char* fmt, ...) IM_FMTARGS(2);

            void Draw(const char* title, bool* p_open = NULL);
        };

        class LogSingleton
        {
        public:
            static ImGuiLogWidget& instance()
            {
                static ImGuiLogWidget widget{};
                return widget;
            }

        private:
            LogSingleton() = default;
            ~LogSingleton() = default;

            LogSingleton(const LogSingleton&) = delete;
            LogSingleton& operator=(const LogSingleton&) = delete;
        };

    } // namespace internal

} // namespace eeng

#endif /* Log_hpp */
