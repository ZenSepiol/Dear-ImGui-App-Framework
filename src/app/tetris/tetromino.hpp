#include "datatypes.hpp"

using LocalGrid = std::array<std::array<Cell, 5>, 5>;

class Tetromino
{
  public:
    Tetromino(){};
    virtual ~Tetromino() = default;

    // Returns all occupied cells
    Board GetGrid(bool rotate_left = false, bool rotate_right = false)
    {
        Board board;
        Rotation local_rotation = rotation;
        if (rotate_left)
        {
            if (rotation == Rotation::One)
                local_rotation = Rotation::Two;
            else if (rotation == Rotation::Two)
                local_rotation = Rotation::Three;
            else if (rotation == Rotation::Three)
                local_rotation = Rotation::Four;
            else if (rotation == Rotation::Four)
                local_rotation = Rotation::One;
        }
        if (rotate_right)
        {
            if (rotation == Rotation::One)
                local_rotation = Rotation::Four;
            else if (rotation == Rotation::Two)
                local_rotation = Rotation::One;
            else if (rotation == Rotation::Three)
                local_rotation = Rotation::Two;
            else if (rotation == Rotation::Four)
                local_rotation = Rotation::Three;
        }

        LocalGrid local_grid = GetLocalGrid(local_rotation);

        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 0; row < board[0].size(); ++row)
            {
                board[col][row] = Cell(false);
            }
        }

        for (int col = 0; col < local_grid.size(); ++col)
        {
            for (int row = 0; row < local_grid[0].size(); ++row)
            {
                if (col + piece_col >= 10 || col + piece_col < 0)
                    continue;
                if (row + piece_row >= 22 || row + piece_row < 0)
                    continue;

                board[col + piece_col][row + piece_row] = local_grid[col][row];
            }
        }

        return board;
    }

    // Checks if rotation is possible and rotates
    void RotateLeft(Board board)
    {
        if (CollisionCheck(board, 0, 0, true, false))
            return;

        if (rotation == Rotation::One)
            rotation = Rotation::Two;
        else if (rotation == Rotation::Two)
            rotation = Rotation::Three;
        else if (rotation == Rotation::Three)
            rotation = Rotation::Four;
        else if (rotation == Rotation::Four)
            rotation = Rotation::One;
    }

    void RotateRight(Board board)
    {
        if (CollisionCheck(board, 0, 0, false, true))
            return;

        if (rotation == Rotation::One)
            rotation = Rotation::Four;
        else if (rotation == Rotation::Two)
            rotation = Rotation::One;
        else if (rotation == Rotation::Three)
            rotation = Rotation::Two;
        else if (rotation == Rotation::Four)
            rotation = Rotation::Three;
    }

    // Checks if move is possible and moves
    void MoveLeft(Board board)
    {
        if (CollisionCheck(board, 0, -1))
            return;

        piece_col -= 1;
    }

    void MoveRight(Board board)
    {
        if (CollisionCheck(board, 0, +1))
            return;

        piece_col += 1;
    }

    // Lets the piece fall one square
    // Returns false if falling is not possible
    bool Fall(Board board)
    {
        if (CollisionCheck(board, 1, 0))
            return false;

        piece_row += 1;
        return true;
    }

    LocalGrid UpdateColor(LocalGrid local_grid, ImVec4 color)
    {
        for (int col = 0; col < local_grid.size(); ++col)
        {
            for (int row = 0; row < local_grid[0].size(); ++row)
            {
                if (local_grid[col][row].filled == true)
                {
                    local_grid[col][row].color = ImGui::GetColorU32(color);
                }
            }
        }
        return local_grid;
    }

    int piece_row = 0;
    int piece_col = 0;

  private:
    // Returns true if there is a collision between self and the board
    bool CollisionCheck(Board board, int row_offset = 0, int col_offset = 0, bool rotate_left = false,
                        bool rotate_right = false)
    {
        Board self = GetGrid(rotate_left, rotate_right);
        for (int col = 0; col < board.size(); ++col)
        {
            for (int row = 2; row < board[0].size(); ++row)
            {
                // Check if the fields are accessible
                if (row + row_offset >= 22 || row + row_offset < 0)
                    continue;
                if (col + col_offset >= 22 || col + col_offset < 0)
                    continue;

                // Check the self board with offset against the input
                if (self[col][row].filled)
                {
                    if (board[col + col_offset][row + row_offset].filled)
                        return true;
                }
            }
        }

        // Check if the piece is outside the board
        Rotation local_rotation = rotation;
        if (rotate_left)
        {
            if (rotation == Rotation::One)
                local_rotation = Rotation::Two;
            else if (rotation == Rotation::Two)
                local_rotation = Rotation::Three;
            else if (rotation == Rotation::Three)
                local_rotation = Rotation::Four;
            else if (rotation == Rotation::Four)
                local_rotation = Rotation::One;
        }
        if (rotate_right)
        {
            if (rotation == Rotation::One)
                local_rotation = Rotation::Four;
            else if (rotation == Rotation::Two)
                local_rotation = Rotation::One;
            else if (rotation == Rotation::Three)
                local_rotation = Rotation::Two;
            else if (rotation == Rotation::Four)
                local_rotation = Rotation::Three;
        }

        LocalGrid local_grid = GetLocalGrid(local_rotation);
        for (int col = 0; col < local_grid.size(); ++col)
        {
            for (int row = 0; row < local_grid[0].size(); ++row)
            {
                if (local_grid[col][row].filled == true)
                {
                    if (col + piece_col + col_offset >= 10 || col + piece_col + col_offset < 0)
                        return true;
                    if (row + piece_row + row_offset >= 22 || row + piece_row + row_offset < 0)
                        return true;
                }
            }
        }

        return false;
    }

    // The local grid of the piece based on the rotation
    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        if (local_rotation == Rotation::One)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Two)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Three)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Four)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }

        return UpdateColor(local_grid, ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 1.f));
    }

    Rotation rotation = Rotation::Four;
};

class I_Tetromino : public Tetromino
{
  public:
    I_Tetromino()
    {
        piece_row = -1;
        piece_col = 3;
    }

    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        if (local_rotation == Rotation::One)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(true), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Two)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Three)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(true), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Four)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }

        return UpdateColor(local_grid, ImVec4(0.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1.f));
    }
};

class L_Tetromino : public Tetromino
{
  public:
    L_Tetromino()
    {
        piece_row = -2;
        piece_col = 3;
    }

    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        if (local_rotation == Rotation::One)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Two)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Three)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Four)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }

        return UpdateColor(local_grid, ImVec4(255.f / 255.f, 126.f / 255.f, 0.f / 255.f, 1.f));
    }
};

class J_Tetromino : public Tetromino
{
  public:
    J_Tetromino()
    {
        piece_row = -2;
        piece_col = 3;
    }

    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        if (local_rotation == Rotation::One)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Two)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Three)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Four)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }

        return UpdateColor(local_grid, ImVec4(0.f / 255.f, 0.f / 255.f, 255.f / 255.f, 1.f));
    }
};

class O_Tetromino : public Tetromino
{
  public:
    O_Tetromino()
    {
        piece_row = -2;
        piece_col = 4;
    }

    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        local_grid = {{{{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                       {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                       {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                       {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                       {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};

        return UpdateColor(local_grid, ImVec4(255.f / 255.f, 255.f / 255.f, 0.f / 255.f, 1.f));
    }
};


class Z_Tetromino : public Tetromino
{
  public:
    Z_Tetromino()
    {
        piece_row = -2;
        piece_col = 3;
    }

    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        if (local_rotation == Rotation::One)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Two)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Three)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Four)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }

        return UpdateColor(local_grid, ImVec4(255.f / 255.f, 0.f / 255.f, 0.f / 255.f, 1.f));
    }
};

class S_Tetromino : public Tetromino
{
  public:
    S_Tetromino()
    {
        piece_row = -2;
        piece_col = 3;
    }

    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        if (local_rotation == Rotation::One)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Two)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Three)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Four)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }

        return UpdateColor(local_grid, ImVec4(0.f / 255.f, 255.f / 255.f, 0.f / 255.f, 1.f));
    }
};

class T_Tetromino : public Tetromino
{
  public:
    T_Tetromino()
    {
        piece_row = -2;
        piece_col = 3;
    }

    virtual LocalGrid GetLocalGrid(Rotation local_rotation)
    {
        LocalGrid local_grid;
        if (local_rotation == Rotation::One)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Two)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Three)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(true)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }
        else if (local_rotation == Rotation::Four)
        {
            local_grid = {{{{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(true), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(true), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}},
                           {{Cell(false), Cell(false), Cell(false), Cell(false), Cell(false)}}}};
        }

        return UpdateColor(local_grid, ImVec4(126.f / 255.f, 0.f / 255.f, 126.f / 255.f, 1.f));
    }
};