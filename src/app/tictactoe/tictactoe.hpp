#include <iostream>
#include <stdio.h>

#include "app_base/app_base.hpp"

enum class CellState
{
    Empty,
    Circle,
    Cross
};

class TicTacToe : public AppBase<TicTacToe>
{
  public:
    TicTacToe(){};
    ~TicTacToe() = default;

    void StartUp()
    {
    }

    void Update()
    {
        ImGui::Begin("Tool");

        ImGui::End();

        // ImDrawList* draw = ImGui::GetBackgroundDrawList();

        for (int row = 0; row < 3; ++row)
        {
            for (int col = 0; col < 3; ++col)
            {
                DrawCell(row, col);
            }
        }
    }

    void DrawCell(int row, int col)
    {
        int cell_width  = 100;
        int cell_height = cell_width;

        CellState cell_value = GetCell(row, col);

        ImVec2 top_left     = ImVec2(cell_width * col, cell_height * row);
        ImVec2 bottom_right = ImVec2(cell_width * (col + 1), cell_height * (row + 1));

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        draw->AddRect(top_left, bottom_right, ImGui::GetColorU32(ImGuiCol_Text));

        int symbol_size = 50;
        ImVec2 center   = ImVec2(cell_width / 2 + top_left.x, cell_height / 2 + top_left.y);
        if (cell_value == CellState::Circle)
        {
            draw->AddCircle(center, symbol_size / 2, ImGui::GetColorU32(ImGuiCol_Text));
        }
        else if (cell_value == CellState::Cross)
        {
            ImVec2 tl(center.x - symbol_size / 2, center.y - symbol_size / 2);
            ImVec2 br(center.x + symbol_size / 2, center.y + symbol_size / 2);
            draw->AddLine(tl, br, ImGui::GetColorU32(ImGuiCol_Text));

            ImVec2 tr(center.x + symbol_size / 2, center.y - symbol_size / 2);
            ImVec2 bl(center.x - symbol_size / 2, center.y + symbol_size / 2);
            draw->AddLine(tr, bl, ImGui::GetColorU32(ImGuiCol_Text));
        }
        else
        {
        }

        if (mouse_x > top_left.x && mouse_x < bottom_right.x && mouse_y > top_left.y && mouse_y < bottom_right.y &&
            cell_value == CellState::Empty)
        {
            draw->AddRect(top_left, bottom_right, ImGui::GetColorU32(ImGuiCol_Text), 0.0f, 0, 4.0);
        }
    }

    CellState GetCell(int row, int col)
    {
        return game_state[row][col];
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        // ONLY forward mouse data to your underlying app/game.
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
            if (button == ImGuiMouseButton_Left && action == 1)
            {
                clicked = true;
            }
            else if (button == ImGuiMouseButton_Left && action == 0)
            {
                clicked = false;
            }
        }
    }

    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        // ONLY forward mouse data to your underlying app/game.
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
            mouse_x = xpos;
            mouse_y = ypos;
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
    std::array<std::array<CellState, 3>, 3> game_state = {
        {{{CellState::Empty, CellState::Empty, CellState::Empty}},
         {{CellState::Cross, CellState::Cross, CellState::Cross}},
         {{CellState::Circle, CellState::Circle, CellState::Circle}}}};

    static int mouse_x;
    static int mouse_y;

    static bool clicked;
};

int TicTacToe::mouse_x = 0;
int TicTacToe::mouse_y = 0;

bool TicTacToe::clicked = false;
