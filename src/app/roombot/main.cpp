#include <iostream>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>

#include "app_base/app_base.hpp"
#include "q_learning/q_learning.hpp"
#include "room_bot/room_bot.hpp"

enum class GuiState
{
    learning,
    show_run
};

class Map
{
  public:
    static const int size_x = 6;
    static const int size_y = 6;

    static constexpr std::array<std::array<bool, size_x>, size_y> map = {{{{true, true, true, true, true, true}},
                                                                          {{true, true, true, true, true, true}},
                                                                          {{true, false, false, false, false, false}},
                                                                          {{true, false, true, false, true, true}},
                                                                          {{true, false, true, false, true, true}},
                                                                          {{true, true, true, true, true, true}}}};
};

class MyApp : public AppBase<MyApp>
{
  public:
    MyApp()
    {
        static ImVec4 custom_color[2] = {ImVec4(23.f / 255.f, 123.f / 255.f, 166.f / 255.f, 1.f),
                                         ImVec4(1.0, 1.0, 0.8, 1.0)};
        auto index                    = ImPlot::AddColormap("Zen Sepiol", custom_color, 2);
        ImPlot::GetStyle().Colormap   = index;
    };
    ~MyApp() = default;

    virtual void StartUp() final
    {
    }

    virtual void Update() final
    {
        if (gui_state == GuiState::learning)
        {
            float total_reward = 0.0;
            int clean_tiles    = 0;
            int moves_made     = 0;

            for (int i = 0; i < episodes_per_epoch * initial_train; ++i)
            {
                q_learning.Reset();
                while ((q_learning.GetState().GetNumberTiles(TileState::dirty) != 0) &&
                       (q_learning.number_of_system_updates < moves))
                {
                    q_learning.Update();
                    total_reward += q_learning.last_reward;
                }
                q_learning.Update();
                total_reward += q_learning.last_reward;

                clean_tiles += q_learning.GetState().GetNumberTiles(TileState::clean);
                moves_made += q_learning.number_of_system_updates;
            }
            initial_train = 1;

            explore_rate.push_back(q_learning.explore_rate);
            total_reward_per_epoch.push_back(total_reward / static_cast<float>(episodes_per_epoch));
            clean_tiles_average.push_back(clean_tiles / static_cast<float>(episodes_per_epoch));
            moves_made_average.push_back(moves_made / static_cast<float>(episodes_per_epoch));

            q_learning.Reset();
            q_learning.print = true;
            gui_state        = GuiState::show_run;
        }
        else if (gui_state == GuiState::show_run)
        {
            float time_for_frame = ImGui::GetIO().DeltaTime;
            time_for_frame_acc += time_for_frame;

            if (time_for_frame_acc >= time_for_frame_des)
            {
                time_for_frame_acc = 0.f;
                if ((q_learning.GetState().GetNumberTiles(TileState::dirty) != 0) &&
                    (q_learning.number_of_system_updates < moves))
                {
                    q_learning.Update();
                }
                else
                {
                    gui_state        = GuiState::learning;
                    q_learning.print = false;
                }
            }
        }

        if (ImGui::Begin("Room Bot Statistics"))
        {
            if (ImPlot::BeginSubplots("", 4, 1, ImVec2(1200, 800)))
            {
                if (ImPlot::BeginPlot(""))
                {
                    ImPlot::SetupAxis(ImAxis_X1, "Number of Epochs", ImPlotAxisFlags_AutoFit);
                    ImPlot::SetupAxis(ImAxis_Y1, "Average Clean Tiles");
                    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 28);

                    std::vector<float> local_samples(clean_tiles_average.size());
                    std::generate(local_samples.begin(), local_samples.end(), [n = 0]() mutable { return 1.0 * n++; });
                    ImPlot::PlotLine("Average clean tiles", local_samples.data(), clean_tiles_average.data(),
                                     clean_tiles_average.size());
                    ImPlot::EndPlot();
                }
                if (ImPlot::BeginPlot(""))
                {
                    ImPlot::SetupAxis(ImAxis_X1, "Number of Epochs", ImPlotAxisFlags_AutoFit);
                    ImPlot::SetupAxis(ImAxis_Y1, "Average Moves Made");
                    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 101);

                    std::vector<float> local_samples(moves_made_average.size());
                    std::generate(local_samples.begin(), local_samples.end(), [n = 0]() mutable { return 1.0 * n++; });
                    ImPlot::PlotLine("Average moves made", local_samples.data(), moves_made_average.data(),
                                     moves_made_average.size());
                    ImPlot::EndPlot();
                }
                if (ImPlot::BeginPlot(""))
                {
                    ImPlot::SetupAxis(ImAxis_X1, "Number of Epochs", ImPlotAxisFlags_AutoFit);
                    ImPlot::SetupAxis(ImAxis_Y1, "Explore Rate");
                    ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1.01);

                    std::vector<float> local_samples(explore_rate.size());
                    std::generate(local_samples.begin(), local_samples.end(), [n = 0]() mutable { return 1.0 * n++; });
                    ImPlot::PlotLine("Explore Rate", local_samples.data(), explore_rate.data(), explore_rate.size());
                    ImPlot::EndPlot();
                }
                ImPlot::EndSubplots();
            }
        }
        ImGui::End();

        if (ImGui::Begin("Room Bot Run"))
        {
            RoomBotState<Map> state = q_learning.GetState();

            int tile_size = 60;
            int gap       = 1;
            int begin     = 50;
            for (int row = 0; row < Map::size_y; ++row)
            {
                for (int col = 0; col < Map::size_x; ++col)
                {
                    ImVec4 color;
                    if (Map::map[col][row])
                    {
                        if (state.tiles[col][row] == TileState::clean)
                        {
                            color = ImVec4(0.18, 0.67, 0.17, 1.0f);
                        }
                        else
                        {
                            color = ImVec4(0.74, 0.13, 0.08, 1.0f);
                        }
                    }
                    else
                    {
                        color = ImVec4(0.11, 0.11, 0.13, 0.5f);
                    }
                    RectangleRelative(ImVec2(begin + tile_size * row, begin + tile_size * col),
                                      ImVec2(tile_size - gap, tile_size - gap), color);
                }
            }
            ImVec4 color = ImVec4(0.99, 0.88, 0.69, 0.5f);
            RectangleRelative(
                ImVec2(begin + tile_size * state.bot_position_y, begin + tile_size * state.bot_position_x),
                ImVec2(tile_size - gap, tile_size - gap), color);
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
    void RectangleRelative(const ImVec2& position_relative, const ImVec2 size, const ImVec4& color)
    {
        const ImU32 color_int = ImColor(color);
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const auto pos        = ImGui::GetWindowPos();
        const auto abs_pos    = ImVec2(pos.x + position_relative.x, pos.y + position_relative.y);

        draw_list->AddRectFilled(abs_pos, ImVec2(abs_pos.x + size.x, abs_pos.y + size.y), color_int);
    }

    QLearning<RoomBot<Map>> q_learning{RoomBot<Map>()};

    GuiState gui_state = GuiState::show_run;

    int moves              = 100;
    int episodes_per_epoch = 20000;
    int initial_train      = 1;

    std::vector<float> total_reward_per_epoch;
    std::vector<float> clean_tiles_average;
    std::vector<float> explore_rate;
    std::vector<float> moves_made_average;

    float time_for_frame_acc = 0.f;
    float time_for_frame_des = 0.1f;
};

int main(int, char**)
{
    MyApp app;
    app.Run();

    return 0;
}