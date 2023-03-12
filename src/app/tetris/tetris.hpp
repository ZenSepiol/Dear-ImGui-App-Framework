#include "app_base/app_base.hpp"
#include "datatypes.hpp"
#include "tetromino.hpp"
#include <memory>
#include <random>
#include <stdio.h>

class Tetris : public AppBase<Tetris>
{
  public:
    Tetris(){};
    virtual ~Tetris() = default;

    virtual void StartUp()
    {
        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 0; row < board[0].size(); ++row)
            {
                board[col][row] = Cell();
            }
        }

        tetromino      = RandomTetromino();
        next_tetromino = RandomTetromino();
    }

    void Update()
    {
        ImGuiIO& io      = ImGui::GetIO();
        float delta_time = io.DeltaTime;

        if (!game_over)
        {
            if (time_till_fall - delta_time < 0)
            {
                time_till_fall = 0.5f * 1000.0f / static_cast<float>(score + 1000);

                // Let the tetromino fall
                if (!tetromino->Fall(board))
                {
                    MergeBoards(tetromino->GetGrid());
                    GameOverCheck();
                    DeleteLines();

                    tetromino      = std::move(next_tetromino);
                    next_tetromino = RandomTetromino();
                }
            }
            else
            {
                time_till_fall -= delta_time;
            }

            if (left_rotate)
            {
                tetromino->RotateLeft(board);
                left_rotate = false;
            }
            if (right_rotate)
            {
                tetromino->RotateRight(board);
                right_rotate = false;
            }
            if (right_move)
            {
                tetromino->MoveRight(board);
                right_move = false;
            }
            if (left_move)
            {
                tetromino->MoveLeft(board);
                left_move = false;
            }
            if (down)
            {
                while (tetromino->Fall(board))
                {
                }
                down = false;
            }
        }

        // Draw the next tetromino
        int offset = cell_size * (board.size() + 1);
        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 0; row < board[0].size(); ++row)
            {
                ImVec2 top_left(offset + col * cell_size, (row + 3) * cell_size);
                ImVec2 bottom_right(offset + (col + 1) * cell_size, (row + 4) * cell_size);

                ImDrawList* draw = ImGui::GetBackgroundDrawList();
                if (next_tetromino->GetGrid()[col][row].filled == true)
                {
                    draw->AddRectFilled(top_left, bottom_right, next_tetromino->GetGrid()[col][row].color);
                }
            }
        }

        // Draw the score
        const char* text_str = "Score";
        const ImVec2 text_pos = ImVec2(offset, 300);
        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        draw->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5, text_pos, IM_COL32_WHITE, text_str);

        char score_string[20];
        sprintf(score_string, "%i", score);
        const ImVec2 score_pos = ImVec2(offset, 300+25);
        draw->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5, score_pos, IM_COL32_WHITE, score_string);

        // Draw the tetromino
        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 0; row < board[0].size(); ++row)
            {
                ImVec2 top_left(col * cell_size, row * cell_size);
                ImVec2 bottom_right((col + 1) * cell_size, (row + 1) * cell_size);

                ImDrawList* draw = ImGui::GetBackgroundDrawList();
                if (tetromino->GetGrid()[col][row].filled == true)
                {
                    draw->AddRectFilled(top_left, bottom_right, tetromino->GetGrid()[col][row].color);
                }
            }
        }

        // Draw the board
        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 2; row < board[0].size(); ++row)
            {
                ImVec2 top_left(col * cell_size, row * cell_size);
                ImVec2 bottom_right((col + 1) * cell_size, (row + 1) * cell_size);

                ImDrawList* draw = ImGui::GetBackgroundDrawList();
                draw->AddRect(top_left, bottom_right, ImGui::GetColorU32(ImGuiCol_Text));

                if (GetCell(row, col).filled == true)
                {
                    draw->AddRectFilled(top_left, bottom_right, GetCell(row, col).color);
                }
            }
        }
    }

    std::unique_ptr<Tetromino> RandomTetromino()
    {
        static std::random_device randDev;
        static std::mt19937 twister(randDev());
        static std::uniform_int_distribution<int> dist;

        dist.param(std::uniform_int_distribution<int>::param_type(0, 6));
        auto rand = dist(twister);
        if (rand == 0)
        {
            return std::make_unique<I_Tetromino>();
        }
        else if (rand == 1)
        {
            return std::make_unique<L_Tetromino>();
        }
        else if (rand == 2)
        {
            return std::make_unique<S_Tetromino>();
        }
        else if (rand == 3)
        {
            return std::make_unique<Z_Tetromino>();
        }
        else if (rand == 4)
        {
            return std::make_unique<T_Tetromino>();
        }
        else if (rand == 5)
        {
            return std::make_unique<O_Tetromino>();
        }
        else if (rand == 6)
        {
            return std::make_unique<J_Tetromino>();
        }

        return std::make_unique<Tetromino>();
    }

    void DeleteLines()
    {
        int current_row   = board[0].size() - 1;
        int lines_cleared = 0;
        while (current_row)
        {
            bool all_filled = true;

            for (int col = 0; col < board.size(); ++col)
            {
                if (!board[col][current_row].filled)
                    all_filled = false;
            }

            if (all_filled)
            {
                lines_cleared++;
                for (int col = 0; col < board.size(); ++col)
                {
                    for (int row = current_row; row > 2; --row)
                    {
                        board[col][row] = board[col][row - 1];
                    }
                }
            }
            else
            {
                current_row--;
            }
        }

        if (lines_cleared == 1)
        {
            score += 40;
        }
        else if (lines_cleared == 2)
        {
            score += 100;
        }
        else if (lines_cleared == 3)
        {
            score += 300;
        }
        else if (lines_cleared == 4)
        {
            score += 1200;
        }
    }

    Cell GetCell(int row, int col)
    {
        return board[col][row];
    }

    void MergeBoards(Board other)
    {
        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 0; row < board[0].size(); ++row)
            {
                if (other[col][row].filled == true)
                {
                    board[col][row] = other[col][row];
                }
            }
        }
    }

    void GameOverCheck()
    {
        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 0; row < 2; ++row)
            {
                if (board[col][row].filled == true)
                    game_over = true;
            }
        }
    }

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
        }
    }

    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureMouse)
        {
        }
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard)
        {
            if (key == GLFW_KEY_A && action == GLFW_PRESS)
            {
                left_move = true;
            }
            if (key == GLFW_KEY_A && action != GLFW_PRESS)
            {
                left_move = false;
            }

            if (key == GLFW_KEY_D && action == GLFW_PRESS)
            {
                right_move = true;
            }
            if (key == GLFW_KEY_D && action != GLFW_PRESS)
            {
                right_move = false;
            }

            if (key == GLFW_KEY_R && action == GLFW_PRESS)
            {
                left_rotate = true;
            }
            if (key == GLFW_KEY_R && action != GLFW_PRESS)
            {
                left_rotate = false;
            }

            if (key == GLFW_KEY_F && action == GLFW_PRESS)
            {
                right_rotate = true;
            }
            if (key == GLFW_KEY_F && action != GLFW_PRESS)
            {
                right_rotate = false;
            }

            if (key == GLFW_KEY_S && action == GLFW_PRESS)
            {
                down = true;
            }
            if (key == GLFW_KEY_S && action != GLFW_PRESS)
            {
                down = false;
            }
        }
    }

  private:
    Board board;

    const int cell_size = 30;

    std::unique_ptr<Tetromino> tetromino;
    std::unique_ptr<Tetromino> next_tetromino;

    float time_till_fall = 1.0;

    bool game_over = false;

    static bool left_move;
    static bool right_move;
    static bool left_rotate;
    static bool right_rotate;
    static bool down;

    int score = 0;
};

bool Tetris::left_move    = false;
bool Tetris::right_move   = false;
bool Tetris::left_rotate  = false;
bool Tetris::right_rotate = false;
bool Tetris::down         = false;