#include "app_base/app_base.hpp"
#include "curl_wrapper/curl_wrapper.hpp"
#include "implot_internal.h"
#include "spot_data/date.h"
#include "spot_data/spot_data.hpp"
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

using namespace std::chrono;

class App : public AppBase<App>
{
  public:
    App(){};
    virtual ~App() = default;

    // Anything that needs to be called once OUTSIDE of the main application loop
    void StartUp()
    {
        ParseData();
    }

    // Anything that needs to be called cyclically INSIDE of the main application loop
    void Update()
    {
        // ImGui::ShowDemoWindow();

        if (!ImGui::Begin("Stock Market Graph"))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return;
        }

        if (ImGui::TreeNode("Input Data"))
        {
            ImGui::InputText("Starting Date", starting_date, 64);
            // ImGui::Text("Parsed Starting Date");
            // ImGui::Text(date::format("%Y-%m-%d", starting_tp).c_str());
            ImGui::Separator();

            ImGui::InputText("Ending date", ending_date, 64);
            // ImGui::Text("Parsed Ending Date");
            // ImGui::Text(date::format("%Y-%m-%d", ending_tp).c_str());
            ImGui::Separator();

            ImGui::InputText("Symbol", symbol, 64);
            // ImGui::Text("Parsed Symbol");
            // ImGui::Text(symbol_str.c_str());
            ImGui::Separator();

            if (ImGui::Button("Parse Input"))
                ParseData();
            ImGui::TreePop();
        }

        if (ImPlot::BeginPlot("Graph", ImVec2(-1, -1), ImPlotFlags_NoLegend))
        {
            ImVec4 red           = ImVec4(0.000f, 1.000f, 0.441f, 1.000f);
            ImVec4 green         = ImVec4(0.853f, 0.050f, 0.310f, 1.000f);
            double width_percent = 0.25;

            ImPlot::SetupAxis(ImAxis_X1, "Date", ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxis(ImAxis_Y1, "Value", ImPlotAxisFlags_AutoFit);
            ImPlot::SetupAxisFormat(ImAxis_Y1, "$%.0f");
            ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);

            // get ImGui window DrawList
            ImDrawList* draw_list = ImPlot::GetPlotDrawList();

            auto conv = [](const system_clock::time_point& tp) {
                return duration_cast<seconds>(tp.time_since_epoch()).count();
            };

            int count      = spot_data.spots.size();
            auto start_it  = spot_data.spots.begin();
            auto second_it = std::next(start_it, 1);
            double half_width =
                count > 1 ? (conv(start_it->first) - conv(second_it->first)) * width_percent : width_percent;

            if (ImPlot::BeginItem("Graph"))
            {
                // ImPlot::GetCurrentItem()->Color = IM_COL32(64, 64, 64, 255);
                if (ImPlot::FitThisFrame())
                {
                    for (const auto& [key, value] : spot_data.spots)
                    {
                        ImPlot::FitPoint(ImPlotPoint(conv(key), value.low));
                        ImPlot::FitPoint(ImPlotPoint(conv(key), value.high));
                    }
                }

                for (const auto& [key, value] : spot_data.spots)
                {
                    ImVec2 open_pos  = ImPlot::PlotToPixels(conv(key) - half_width, value.open);
                    ImVec2 close_pos = ImPlot::PlotToPixels(conv(key) + half_width + 1, value.close);
                    ImVec2 low_pos   = ImPlot::PlotToPixels(conv(key), value.low);
                    ImVec2 high_pos  = ImPlot::PlotToPixels(conv(key), value.high);
                    ImU32 color      = ImGui::GetColorU32(value.open > value.close ? green : red);
                    draw_list->AddLine(low_pos, high_pos, color);
                    draw_list->AddRectFilled(open_pos, close_pos, color);
                }

                ImPlot::EndItem();
            }
            ImPlot::EndPlot();
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

    void ParseData()
    {
        std::istringstream in{starting_date};
        in >> date::parse("%F", starting_tp);

        std::istringstream in2{ending_date};
        in2 >> date::parse("%F", ending_tp);

        symbol_str = std::string(symbol);

        CurlWrapper wrapper;
        std::string url = wrapper.GenerateURL(symbol_str, starting_tp, ending_tp, "1d");
        wrapper.DownloadCSV(url);

        spot_data = SpotData(wrapper.GetResponse());
        for (const auto& [key, value] : spot_data.spots)
        {
            std::cout << "Time: " << date::format("%Y-%m-%d", key) << " High: " << value.high << std::endl;
        }
    }

  private:
    char starting_date[64] = "2023-01-01";
    char ending_date[64]   = "2023-02-27";
    char symbol[64]        = "^GSPC";

    system_clock::time_point starting_tp;
    system_clock::time_point ending_tp;
    std::string symbol_str;

    SpotData spot_data;
};