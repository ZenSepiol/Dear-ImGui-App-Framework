#include "app_base/app_base.hpp"
#include "implot_internal.h"
#include <future>
#include <iostream>
#include <string>
#include <vector>

class App : public AppBase<App>
{
  public:
    App(){};
    virtual ~App() = default;

    // Anything that needs to be called once OUTSIDE of the main application loop
    void StartUp()
    {
        for (int i = 0; i < 200; ++i)
        {
            frame_times.push_back(0.f);
        }
    }

    int MyCalculation() const
    {
        volatile int x = 0;
        for (int i = 0; i < 500000000; ++i)
        {
            x++;
        }

        static int result = 0;
        result++;
        return result;
    }

    // Anything that needs to be called cyclically INSIDE of the main application loop
    void Update()
    {
        // ImGui::ShowDemoWindow();

        frame_times.erase(frame_times.begin());
        frame_times.push_back(1.f / ImGui::GetIO().DeltaTime);

        if (!ImGui::Begin("Long running task"))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }

        if (ImGui::Button("Long Calculation"))
            future_result = std::async(std::launch::async, &App::MyCalculation, this);

        if (future_result.valid())
            if (future_result.wait_for(std::chrono::milliseconds(1)) == std::future_status::ready)
                last_result = future_result.get();

        ImGui::Text("My result is: %i", last_result);

        if (ImPlot::BeginSubplots("", 1, 1, ImVec2(1200, 800)))
        {
            if (ImPlot::BeginPlot(""))
            {
                ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 5.f);

                ImPlot::SetupAxis(ImAxis_X1, "Frame Number", ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxis(ImAxis_Y1, "Frame Rate");
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);

                std::vector<float> local_samples(frame_times.size());
                std::generate(local_samples.begin(), local_samples.end(), [n = 0]() mutable { return 1.0 * n++; });
                ImPlot::PlotLine("Frame Rate", local_samples.data(), frame_times.data(), frame_times.size());
                ImPlot::EndPlot();

                ImPlot::PopStyleVar(ImPlotStyleVar_LineWeight);
            }
            ImPlot::EndSubplots();
        }

        ImGui::End();
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

  private:
    std::vector<float> frame_times;
    int last_result = 0;
    std::future<int> future_result;
};