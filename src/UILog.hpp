
#ifndef UILog_hpp
#define UILog_hpp

// #include <stdio.h>
#include <memory>
#include "imgui.h"

// Adapted from imgui_demo.cpp

struct LogWidget;

class UILog
{
public:
    static std::unique_ptr<LogWidget> log_widget;

    static void log(const char *str);

    static void draw(bool *p_open = nullptr);

    static void clear();
};

#endif /* UILog_hpp */
