#pragma once

#include "room_bot_action.hpp"
#include "room_bot_state.hpp"

template <typename Map>
class RoomBot
{
  public:
    using Action = RoomBotAction;
    using State = RoomBotState<Map>;

    void Update(double time_step, Action action)
    {
        if (action.direction == Direction::left)
        {
            state.bot_position_x--;
        }
        else if (action.direction == Direction::right)
        {
            state.bot_position_x++;
        }
        else if (action.direction == Direction::up)
        {
            state.bot_position_y--;
        }
        else if (action.direction == Direction::down)
        {
            state.bot_position_y++;
        }

        state.tiles[state.bot_position_x][state.bot_position_y] = TileState::clean;
    };

    void Reset()
    {
        state = State();
    };

    State state;
};