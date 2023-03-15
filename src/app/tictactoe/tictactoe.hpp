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
            if (game_over && winner == CellState::Circle)
                draw->AddCircle(center, symbol_size / 2, ImGui::GetColorU32(ImVec4(0.50f, 0.00f, 0.00f, 1.00f)));
            else
                draw->AddCircle(center, symbol_size / 2, ImGui::GetColorU32(ImGuiCol_Text));

        }
        else if (cell_value == CellState::Cross)
        {
            ImVec2 tl(center.x - symbol_size / 2, center.y - symbol_size / 2);
            ImVec2 br(center.x + symbol_size / 2, center.y + symbol_size / 2);
            if (game_over && winner == CellState::Cross)
                draw->AddLine(tl, br, ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.50f, 0.00f, 0.00f, 1.00f))));
            else
                draw->AddLine(tl, br, ImGui::GetColorU32(ImGuiCol_Text));

            ImVec2 tr(center.x + symbol_size / 2, center.y - symbol_size / 2);
            ImVec2 bl(center.x - symbol_size / 2, center.y + symbol_size / 2);
            if (game_over && winner == CellState::Cross)
                draw->AddLine(tr, bl, ImGui::GetColorU32(ImGui::GetColorU32(ImVec4(0.50f, 0.00f, 0.00f, 1.00f))));
            else
                draw->AddLine(tr, bl, ImGui::GetColorU32(ImGuiCol_Text));

        }
        else
        {
        }

        if (mouse_x > top_left.x && mouse_x < bottom_right.x && mouse_y > top_left.y && mouse_y < bottom_right.y &&
            cell_value == CellState::Empty)
        {
            draw->AddRect(top_left, bottom_right, ImGui::GetColorU32(ImGuiCol_Text), 0.0f, 0, 4.0);

            if (clicked)
                UpdateCellState(row, col);

        }
    }

    CellState GetCell(int row, int col)
    {
        return game_state[row][col];
    }

    void UpdateCellState(int row, int col)
    {
        if (game_over)
            return;

        if (GetCell(row, col) != CellState::Empty)
            return;
        
        game_state[row][col] = current_player;

        CheckForGameOver(row, col);
        if (current_player == CellState::Cross)
            current_player = CellState::Circle;
        else
            current_player = CellState::Cross;

    }

    void CheckForGameOver(int row, int col)
    {
        game_over = CheckDiag() || CheckRow(row) || CheckCol(col);
        if (game_over)
            winner = current_player;
    }

    bool CheckRow(int row)
    {
        bool over = true;
        for (int i = 0; i < 3; ++i)
        {
            if (!(GetCell(row, i) == current_player))
                over = false;
        }
        return over;
    }

    bool CheckCol(int col)
    {
        bool over = true;
        for (int i = 0; i < 3; ++i)
        {
            if (!(GetCell(i, col) == current_player))
                over = false;
        }
        return over;
    }

    bool CheckDiag()
    {
        if (GetCell(0,0) == current_player && GetCell(1,1) == current_player && GetCell(2,2) == current_player)
            return true;
        if (GetCell(2,0) == current_player && GetCell(1,1) == current_player && GetCell(0,2) == current_player)
            return true;
        return false; 
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
         {{CellState::Empty, CellState::Empty, CellState::Empty}},
         {{CellState::Empty, CellState::Empty, CellState::Empty}}}};

    static int mouse_x;
    static int mouse_y;

    static bool clicked;

    CellState current_player = CellState::Cross;
    CellState winner = CellState::Empty;
    bool game_over = false;
};

int TicTacToe::mouse_x = 0;
int TicTacToe::mouse_y = 0;

bool TicTacToe::clicked = false;
