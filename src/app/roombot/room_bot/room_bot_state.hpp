#pragma once

#include "room_bot_action.hpp"
#include "room_bot_state.hpp"
#include <algorithm>

enum class TileState
{
    dirty,
    clean
};

// Example Map
// class Map
// {
//   public:
//     static const int size_x = 2;
//     static const int size_y = 2;

//     static constexpr std::array<std::array<bool, size_x>, size_y> map = {{{{true, false}},
//                                                                           {{true, true}}}};
// };

template <typename Map>
struct RoomBotState
{
    unsigned bot_position_x = 0;
    unsigned bot_position_y = 0;

    std::array<std::array<TileState, Map::size_y>, Map::size_x> tiles;

    RoomBotState()
    {
        for (int row = 0; row < Map::size_x; ++row)
        {
            for (int col = 0; col < Map::size_y; ++col)
            {
                tiles[col][row] = TileState::dirty;
            }
        }
        tiles[0][0] = TileState::clean;
    }

    friend bool operator==(const RoomBotState& lhs, const RoomBotState& rhs)
    {
        bool tiles_equal = true;
        for (int row = 0; row < Map::size_x; ++row)
        {
            for (int col = 0; col < Map::size_y; ++col)
            {
                if (lhs.tiles[col][row] != rhs.tiles[col][row])
                {
                    tiles_equal = false;
                }
            }
        }

        bool equal =
            tiles_equal && (lhs.bot_position_x == rhs.bot_position_x) && (lhs.bot_position_y == rhs.bot_position_y);
        return equal;
    };

    std::unordered_set<RoomBotAction> GetActions(void) const
    {
        std::unordered_set<RoomBotAction> set;

        if (bot_position_x > 0)
        {
            if (Map::map[bot_position_x - 1][bot_position_y])
            {
                RoomBotAction action;
                action.direction = Direction::left;
                set.insert(action);
            }
        }
        if (bot_position_x < Map::size_x - 1)
        {
            if (Map::map[bot_position_x + 1][bot_position_y])
            {
                RoomBotAction action;
                action.direction = Direction::right;
                set.insert(action);
            }
        }
        if (bot_position_y > 0)
        {
            if (Map::map[bot_position_x][bot_position_y - 1])
            {
                RoomBotAction action;
                action.direction = Direction::up;
                set.insert(action);
            }
        }
        if (bot_position_y < Map::size_y - 1)
        {
            if (Map::map[bot_position_x][bot_position_y + 1])
            {
                RoomBotAction action;
                action.direction = Direction::down;
                set.insert(action);
            }
        }

        return set;
    }

    float GetReward(const RoomBotState<Map>& previous_state) const
    {
        float reward = -0.1;
        for (int row = 0; row < Map::size_x; ++row)
        {
            for (int col = 0; col < Map::size_y; ++col)
            {
                if (previous_state.tiles[col][row] != tiles[col][row])
                {
                    reward += 1;
                }
            }
        }

        return reward;
    }

    int GetNumberTiles(TileState tile_state) const
    {
        int result = 0;
        for (int row = 0; row < Map::size_x; ++row)
        {
            for (int col = 0; col < Map::size_y; ++col)
            {
                if (tiles[col][row] == tile_state && Map::map[col][row])
                {
                    result++;
                }
            }
        }
        return result;
    }

    void print() const
    {
        std::cout << "bot_position_x: " << bot_position_x << std::endl;
        std::cout << "bot_position_y: " << bot_position_y << std::endl;

        for (auto& row : tiles)
        {
            for (auto& tile : row)
            {
                std::cout << static_cast<int>(tile) << "  ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
};


template <typename Map>
struct std::hash<RoomBotState<Map>>
{
    std::size_t operator()(const RoomBotState<Map>& e) const
    {
        std::size_t hash_result = 0;
        hash_result += e.bot_position_x;
        hash_result = hash_result << 8;

        hash_result += e.bot_position_y;
        hash_result = hash_result << 8;

        for (int row = 0; row < Map::size_x; ++row)
        {
            for (int col = 0; col < Map::size_y; ++col)
            {
                hash_result += static_cast<int>(e.tiles[col][row]);
                hash_result = hash_result << 1;
            }
        }

        return hash_result;
    }
};