#pragma once

#include <algorithm>
#include <unordered_set>

enum class Direction
{
    left,
    right,
    up,
    down
};

struct RoomBotAction
{
    friend bool operator==(const RoomBotAction& lhs, const RoomBotAction& rhs)
    {
        return (lhs.direction == rhs.direction);
    };

    void print() const
    {
        if (direction == Direction::left)
        {
            std::cout << "Direction: Left" << std::endl;
        }
        if (direction == Direction::right)
        {
            std::cout << "Direction: Right" << std::endl;
        }
        if (direction == Direction::up)
        {
            std::cout << "Direction: Up" << std::endl;
        }
        if (direction == Direction::down)
        {
            std::cout << "Direction: Down" << std::endl;
        }
        std::cout << "q_value: " << q_value << std::endl;
        std::cout << std::endl;
    }

    Direction direction = Direction::left;
    mutable float q_value = 0.0;
};

template <>
struct std::hash<RoomBotAction>
{
    std::size_t operator()(const RoomBotAction& e) const
    {
        std::size_t hash_result = static_cast<std::size_t>(e.direction);
        return hash_result;
    }
};