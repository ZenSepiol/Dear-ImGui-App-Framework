#include "app_base/app_base.hpp"
#include "thread_pool/thread_pool.hpp"
#include "implot_internal.h"
#include <iostream>
#include <string>

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
            busy_threads.push_back(0.f);
            open_tasks.push_back(0.f);
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
        if (!ImGui::Begin("Long running task"))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }

        if (ImGui::Button("Long Calculation"))
            futures.push_back(thread_pool.AddTask(std::bind(&App::MyCalculation, this)));

        for (auto& future : futures)
        {
            if (future.valid())
                if (future.wait_for(std::chrono::microseconds(1)) == std::future_status::ready)
                    results.push_back(future.get());
        }

        for (auto& result : results)
        {
            ImGui::Text("My result is: %i", result);
        }
        ImGui::End();

        Statistics();
    }

    void Statistics()
    {
        if (!ImGui::Begin("Statistics"))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }

        busy_threads.erase(busy_threads.begin());
        busy_threads.push_back(thread_pool.busy_threads);

        open_tasks.erase(open_tasks.begin());
        open_tasks.push_back(thread_pool.QueueSize());

        if (ImPlot::BeginSubplots("", 1, 1, ImVec2(1200, 800)))
        {
            if (ImPlot::BeginPlot(""))
            {
                ImPlot::PushStyleVar(ImPlotStyleVar_LineWeight, 5.f);

                ImPlot::SetupAxis(ImAxis_X1, "Frame Number", ImPlotAxisFlags_AutoFit);
                ImPlot::SetupAxis(ImAxis_Y1, "Number");
                ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 10);

                std::vector<float> local_samples(busy_threads.size());
                std::generate(local_samples.begin(), local_samples.end(), [n = 0]() mutable { return 1.0 * n++; });
                ImPlot::PlotLine("Busy Threads", local_samples.data(), busy_threads.data(), busy_threads.size());
                ImPlot::PlotLine("Open Tasks", local_samples.data(), open_tasks.data(), open_tasks.size());
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
    std::vector<float> busy_threads;
    std::vector<float> open_tasks;

    std::vector<std::future<int>> futures;
    std::vector<int> results;

    ThreadPool thread_pool{4};
};