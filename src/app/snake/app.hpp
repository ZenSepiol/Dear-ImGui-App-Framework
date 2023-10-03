#include "app_base/app_base.hpp"
#include <deque>
#include <iostream>
#include <iterator>
#include <random>
#include <set>
#include <string>

template <typename RandomGenerator = std::default_random_engine>
struct random_selector
{
    // On most platforms, you probably want to use std::random_device("/dev/urandom")()
    random_selector(RandomGenerator g = RandomGenerator(std::random_device()())) : gen(g)
    {
    }

    template <typename Iter>
    Iter select(Iter start, Iter end)
    {
        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(gen));
        return start;
    }

    // convenience function
    template <typename Iter>
    Iter operator()(Iter start, Iter end)
    {
        return select(start, end);
    }

    // convenience function that works on anything with a sensible begin() and end(), and returns with a ref to the
    // value type
    template <typename Container>
    auto operator()(const Container& c) -> decltype(*begin(c))&
    {
        return *select(begin(c), end(c));
    }

  private:
    RandomGenerator gen;
};

class App : public AppBase<App>
{
  public:
    App(){};
    virtual ~App() = default;

    // Anything that needs to be called once OUTSIDE of the main application loop
    void StartUp()
    {
        Reset();
    }

    // Anything that needs to be called cyclically INSIDE of the main application loop
    void Update()
    {
        ImGuiIO& io      = ImGui::GetIO();
        float delta_time = io.DeltaTime;
        if (!game_over)
        {
            ChangeDesiredDirection();
            if (next_time_move - delta_time < 0)
            {
                next_time_move = 0.2f * 1.0f / static_cast<float>(score * 0.1 + 1);

                MoveSnake();
                std::cout << "Target: " << target.x << "  " << target.y << "   " << std::endl;
                std::cout << "Head: " << head.x << "  " << head.y << "   " << std::endl;
            }
            else
            {
                next_time_move -= delta_time;
            }
        }

        // Draw the score
        int offset            = cell_size * (grid_size + 1);
        const char* text_str  = "Score";
        const ImVec2 text_pos = ImVec2(offset, 300);
        ImDrawList* draw      = ImGui::GetBackgroundDrawList();
        draw->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5, text_pos, IM_COL32_WHITE, text_str);

        char score_string[20];
        sprintf(score_string, "%i", score);
        const ImVec2 score_pos = ImVec2(offset, 300 + 25);
        draw->AddText(ImGui::GetFont(), ImGui::GetFontSize() + 5, score_pos, IM_COL32_WHITE, score_string);

        // Draw the head
        DrawCell(head.y, head.x, ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 1.f), true);

        // Draw the tail
        for (const auto& element : tail)
        {
            DrawCell(element.y, element.x, ImVec4(252.f / 255.f, 224.f / 255.f, 176.f / 255.f, 0.5f), true);
        }

        // Draw the target
        DrawCell(target.y, target.x, ImVec4(0.18, 0.67, 0.17, 1.0f), true);

        // Draw the board
        for (int col = 0; col < grid_size; ++col)
        {
            for (int row = 0; row < grid_size; ++row)
            {
                DrawCell(col, row, ImVec4(0.18, 0.18, 0.17, 1.0f));
            }
        }
    }

    void DrawCell(int col, int row, const ImVec4& color, bool filled = false)
    {
        const ImU32 color_int = ImColor(color);
        ImVec2 top_left(row * cell_size, col * cell_size);
        ImVec2 bottom_right((row + 1) * cell_size, (col + 1) * cell_size);

        ImDrawList* draw = ImGui::GetBackgroundDrawList();
        if (filled)
            draw->AddRectFilled(top_left, bottom_right, color_int);
        else
            draw->AddRect(top_left, bottom_right, color_int);
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

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        // For Dear ImGui to work it is necessary to queue if the keyboard signal is already processed by Dear ImGui
        // Only if the keyboard is not already captured it should be used here.
        ImGuiIO& io = ImGui::GetIO();
        if (!io.WantCaptureKeyboard)
        {
            if (key == GLFW_KEY_A && action == GLFW_PRESS)
            {
                left_key = true;
            }

            if (key == GLFW_KEY_D && action == GLFW_PRESS)
            {
                right_key = true;
            }

            if (key == GLFW_KEY_S && action == GLFW_PRESS)
            {
                down_key = true;
            }

            if (key == GLFW_KEY_W && action == GLFW_PRESS)
            {
                up_key = true;
            }
        }
    }

  private:
    enum class Direction
    {
        left,
        right,
        up,
        down
    };

    class Point
    {
      public:
        Point() = default;
        Point(int x, int y) : x(x), y(y)
        {
        }

        void Move(Direction direction)
        {
            if (direction == Direction::left)
                x -= 1;
            else if (direction == Direction::right)
                x += 1;
            else if (direction == Direction::up)
                y -= 1;
            else if (direction == Direction::down)
                y += 1;
        }

        bool operator==(const Point& other)
        {
            return x == other.x && y == other.y;
        }

        int x = 0;
        int y = 0;
    };

    void MoveSnake()
    {
        // Move snake
        tail.push_front(head);
        head.Move(desired_direction);

        // Check if collision with target
        if (head == target)
        {
            GenerateTarget();
            score += 1;
        }
        else
        {
            tail.pop_back();
        }

        // Check if collision with itself or wall
        if (std::find(tail.begin(), tail.end(), head) != tail.end())
        {
            game_over = true;
        }
        if (head.x < 0 || head.x >= grid_size || head.y < 0 || head.y >= grid_size)
        {
            game_over = true;
        }
    };

    void GenerateTarget()
    {
        std::vector<Point> target_list_points;
        for (int x = 0; x < grid_size; ++x)
        {
            for (int y = 0; y < grid_size; ++y)
            {
                Point target_list_point = Point(x, y);
                if (std::find(tail.begin(), tail.end(), target_list_point) == tail.end() && target_list_point != head)
                    target_list_points.push_back(target_list_point);
            }
        }

        random_selector<> selector{};
        target = selector(target_list_points);
    };

    void ChangeDesiredDirection()
    {
        if (up_key)
        {
            desired_direction = Direction::up;
            up_key            = false;
        }
        else if (down_key)
        {
            desired_direction = Direction::down;
            down_key          = false;
        }
        else if (left_key)
        {
            desired_direction = Direction::left;
            left_key          = false;
        }
        else if (right_key)
        {
            desired_direction = Direction::right;
            right_key         = false;
        }
    };

    void Reset()
    {
        head              = Point(grid_size / 2, grid_size / 2);
        desired_direction = Direction::left;
        tail.clear();
        score = 0;

        GenerateTarget();
    }

    Point head;
    std::deque<Point> tail;

    Direction desired_direction;

    float next_time_move = 1.0;

    Point target;

    const int grid_size = 20;
    const int cell_size = 30;

    bool game_over = false;
    ;
    int score = 0;

    static bool left_key;
    static bool right_key;
    static bool up_key;
    static bool down_key;
};

bool App::left_key  = false;
bool App::right_key = false;
bool App::up_key    = false;
bool App::down_key  = false;
