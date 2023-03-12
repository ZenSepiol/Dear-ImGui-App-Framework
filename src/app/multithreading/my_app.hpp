#include <algorithm>
#include <iostream>
#include <stdio.h>
#include <vector>

#include <functional>
#include <future>
#include <thread>

#include "app_base/app_base.hpp"

class MyApp : public AppBase<MyApp>
{
  public:
    bool MyCalculation() const
    {
        volatile int x = 0;
        for (int i = 0; i < 10000000; ++i)
        {
            x++;
        }
        return true;
    }

    virtual void StartUp() final
    {
        for (int i = 0; i < 200; ++i)
        {
            frame_times.push_back(0.f);
        }
    }

    virtual void Update() final
    {
        if (calculate)
        {
            MyCalculation();
            MyCalculation();
        }

        if (async)
        {
            std::future<bool> future_one = std::async(std::launch::async, &MyApp::MyCalculation, this);
            std::future<bool> future_two = std::async(std::launch::async, &MyApp::MyCalculation, this);

            bool first_result  = future_one.get();
            bool second_result = future_two.get();
        }

        if (thread)
        {
            std::packaged_task<bool(void)> task_one(std::bind(&MyApp::MyCalculation, this));
            std::packaged_task<bool(void)> task_two(std::bind(&MyApp::MyCalculation, this));

            std::future<bool> future_one = task_one.get_future();
            std::future<bool> future_two = task_two.get_future();

            std::thread thread_one(std::move(task_one));
            std::thread thread_two(std::move(task_two));

            thread_one.join();
            thread_two.join();

            bool first_result  = future_one.get();
            bool second_result = future_two.get();
        }

        frame_times.erase(frame_times.begin());
        frame_times.push_back(1.f / ImGui::GetIO().DeltaTime);

        ImGui::Begin("Tool");

        ImGui::Checkbox("Calculate", &calculate);
        ImGui::Checkbox("Async", &async);
        ImGui::Checkbox("Thread", &thread);

        if (ImPlot::BeginSubplots("", 1, 1, ImVec2(1200, 800)))
        {
            if (ImPlot::BeginPlot(""))
            {
                ImPlot::SetupAxis(ImAxis_X1, "Frame Number", ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxis(ImAxis_Y1, "Frame Rate");
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 100);

                std::vector<float> local_samples(frame_times.size());
                std::generate(local_samples.begin(), local_samples.end(), [n = 0]() mutable { return 1.0 * n++; });
                ImPlot::PlotLine("Frame Rate", local_samples.data(), frame_times.data(), frame_times.size());
                ImPlot::EndPlot();
            }
            ImPlot::EndSubplots();
        }

        ImGui::End();
    };

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

    bool calculate = false;
    bool async     = false;
    bool thread    = false;
};