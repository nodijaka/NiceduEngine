
#ifndef UILog_hpp
#define UILog_hpp

#include <memory>
#include "imgui.h"

namespace eeng {

/// @brief ImGui log widget
///  Adapted from imgui_demo.cpp
struct LogWidget;

class Log
{
public:
    static std::unique_ptr<LogWidget> log_widget;

    // static void log(const char *str);

    /// @brief Add a log item
    /// @param fmt Format string
    static void log(const char *fmt, ...);

    /// @brief Draw the log
    /// @param p_open 
    static void draw(bool *p_open = nullptr);

    /// @brief Clear log
    static void clear();

private:
    static std::string formatString(const char* fmt, va_list args);
};

} // namespace eeng

#endif /* UILog_hpp */
