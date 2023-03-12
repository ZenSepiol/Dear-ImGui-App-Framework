#pragma once

enum class Rotation
{
    One,
    Two,
    Three,
    Four
};

class Cell
{
  public:
    Cell(bool filled = false) : filled(filled){};

    bool filled = false;
    ImU32 color = ImGui::GetColorU32(ImGuiCol_Text);
};

using Board = std::array<std::array<Cell, 22>, 10>;