
#include <iostream>
#include <cstdarg>
#include "Log.hpp"

namespace eeng {

struct LogWidget
{
    ImGuiTextBuffer Buf;
    ImGuiTextFilter Filter;
    ImVector<int> LineOffsets;
    bool AutoScroll;
    bool ScrollToBottom;

    LogWidget()
    {
        AutoScroll = true;
        ScrollToBottom = false;
        Clear();
    }

    void Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void AddLog(const char *fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
        if (AutoScroll)
            ScrollToBottom = true;
    }

    void Draw(const char *title, bool *p_open = NULL)
    {
#if 1
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }
#else
        // Fixate to top-right corner

        ImGuiIO &io = ImGui::GetIO();
        const float DISTANCE = 10.0f;
        static int corner = 1;
        if (corner != -1)
        {
            ImVec2 window_pos = ImVec2((corner & 1) ? io.DisplaySize.x - DISTANCE : DISTANCE, (corner & 2) ? io.DisplaySize.y - DISTANCE : DISTANCE);
            ImVec2 window_pos_pivot = ImVec2((corner & 1) ? 1.0f : 0.0f, (corner & 2) ? 1.0f : 0.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
        }

        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        if (!ImGui::Begin(title, p_open, (corner != -1 ? ImGuiWindowFlags_NoMove : 0) | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav))
        {
            ImGui::End();
            return;
        }
#endif

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            if (ImGui::Checkbox("Auto-scroll", &AutoScroll))
                if (AutoScroll)
                    ScrollToBottom = true;
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        if (clear)
            Clear();
        if (copy)
            ImGui::LogToClipboard();

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        const char *buf = Buf.begin();
        const char *buf_end = Buf.end();
        if (Filter.IsActive())
        {
            // In this example we don't use the clipper when Filter is enabled.
            // This is because we don't have a random access on the result on our filter.
            // A real application processing logs with ten of thousands of entries may want to store the result of search/filter.
            // especially if the filtering function is not trivial (e.g. reg-exp).
            for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
            {
                const char *line_start = buf + LineOffsets[line_no];
                const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                if (Filter.PassFilter(line_start, line_end))
                    ImGui::TextUnformatted(line_start, line_end);
            }
        }
        else
        {
            // The simplest and easy way to display the entire buffer:
            //   ImGui::TextUnformatted(buf_begin, buf_end);
            // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward to skip non-visible lines.
            // Here we instead demonstrate using the clipper to only process lines that are within the visible area.
            // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them on your side is recommended.
            // Using ImGuiListClipper requires A) random access into your data, and B) items all being the  same height,
            // both of which we can handle since we an array pointing to the beginning of each line of text.
            // When using the filter (in the block of code above) we don't have random access into the data to display anymore, which is why we don't use the clipper.
            // Storing or skimming through the search result would make it possible (and would be recommended if you want to search through tens of thousands of entries)
            ImGuiListClipper clipper;
            clipper.Begin(LineOffsets.Size);
            while (clipper.Step())
            {
                for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                {
                    const char *line_start = buf + LineOffsets[line_no];
                    const char *line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    ImGui::TextUnformatted(line_start, line_end);
                }
            }
            clipper.End();
        }
        ImGui::PopStyleVar();

        if (ScrollToBottom)
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;
        ImGui::EndChild();
        ImGui::End();
    }
};

std::unique_ptr<LogWidget> Log::log_widget = std::make_unique<LogWidget>();

void Log::log(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    std::string formattedString = formatString(fmt, args);
    std::cout << formattedString << std::endl;
    va_end(args);

    log_widget->AddLog("[frame#%i] %s\n", ImGui::GetFrameCount(), formattedString.c_str());
}

void Log::draw(bool *p_open)
{
    log_widget->Draw("Log", p_open);
}

void Log::clear()
{
    log_widget->Clear();
}

std::string Log::formatString(const char* fmt, va_list args) 
{
    va_list args_copy;
    va_copy(args_copy, args);

    int length = vsnprintf(NULL, 0, fmt, args_copy);
    char* buffer = new char[length + 1];
    vsnprintf(buffer, length + 1, fmt, args);
    va_end(args_copy);

    std::string formattedString(buffer);
    delete[] buffer;

    return formattedString;
}

} // namespace eeng