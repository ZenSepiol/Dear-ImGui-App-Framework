#include "app_base/app_base.hpp"

#include <chrono>
#include <cmath>
#include <iostream>

class Clock : public AppBase<Clock>
{
  public:
    Clock()
    {
    }
    ~Clock() = default;

    void StartUp()
    {
    }

    void Update()
    {
        // const std::chrono::zoned_time zt("Europe", std::chrono::system_clock::now());
        auto tp = std::chrono::system_clock::now();
        auto dp = floor<std::chrono::days>(tp);

        std::chrono::hh_mm_ss hms{floor<std::chrono::milliseconds>(tp - dp)};

        auto h  = hms.hours();
        auto m  = hms.minutes();
        auto s  = hms.seconds();
        auto ms = hms.subseconds();

        // Get the draw buffer
        ImDrawList* draw = ImGui::GetForegroundDrawList();
        ImU32 color      = ImGui::GetColorU32(ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 1.f));

        float radius       = 200.f;
        float inner_radius = 5.f;
        ImVec2 center(radius, radius);

        // Draw the clock itself
        draw->AddCircle(center, radius, color, 0, 3.f);
        draw->AddCircle(center, inner_radius, color, 0, 3.f);

        for (int i = 0; i < 12; ++i)
        {
            float angle     = (i / 12.f) * 2 * std::numbers::pi;
            float length_in = 20.f;

            DrawAngledLine(angle, radius, length_in, 0.f, color);
        }

        // Draw the clock arms
        ImU32 color_arm = ImGui::GetColorU32(ImVec4(252.f / 255.f, 120.f / 255.f, 120.f / 255.f, 1.f));
        ImU32 color_ms  = ImGui::GetColorU32(ImVec4(120.f / 255.f, 120.f / 255.f, 240.f / 255.f, 1.f));

        float milliseconds_max  = 1000.f;
        auto milliseconds_ratio = ms.count() / milliseconds_max;

        float seconds_max  = 60.f;
        auto seconds_ratio = (s.count() + milliseconds_ratio) / seconds_max;

        float minutes_max  = 60.f;
        auto minutes_ratio = (m.count() + seconds_ratio) / minutes_max;

        float hours_max  = 12.f;
        auto hours_ratio = (h.count() + minutes_ratio) / hours_max;

        DrawAngledLine(hours_ratio * 2.f * std::numbers::pi, radius, radius - inner_radius, 60.f, color_arm);
        DrawAngledLine(minutes_ratio * 2.f * std::numbers::pi, radius, radius - inner_radius, 20.f, color_arm);
        DrawAngledLine(seconds_ratio * 2.f * std::numbers::pi, radius, radius - inner_radius, 20.f, color_arm);
        DrawAngledLine(milliseconds_ratio * 2.f * std::numbers::pi, radius, 30.f, 0.f, color_ms);
    }

    void DrawAngledLine(float angle, float radius, float length_in, float length_out, ImU32 color)
    {
        float x1 = std::sin(-angle + std::numbers::pi) * (radius - length_out) + radius;
        float y1 = std::cos(-angle + std::numbers::pi) * (radius - length_out) + radius;

        float x2 = std::sin(-angle + std::numbers::pi) * (radius - length_in) + radius;
        float y2 = std::cos(-angle + std::numbers::pi) * (radius - length_in) + radius;

        ImDrawList* draw = ImGui::GetForegroundDrawList();
        draw->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color, 3.f);
    }

    // The callbacks are updated and called BEFORE the Update loop is entered
    // It can be assumed that inside the Update loop all callbacks have already been processed
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        // For Dear ImGui to work it is necessary to queue if the mouse signal is already processed by Dear ImGui
        // Only if the mouse is not already captured it should be used here.
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
        }
    }

    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        // For Dear ImGui to work it is necessary to queue if the mouse signal is already processed by Dear ImGui
        // Only if the mouse is not already captured it should be used here.
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
        }
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int actions, int mods)
    {
        // For Dear ImGui to work it is necessary to queue if the keyboard signal is already processed by Dear ImGui
        // Only if the keyboard is not already captured it should be used here.
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard)
        {
        }
    }
};