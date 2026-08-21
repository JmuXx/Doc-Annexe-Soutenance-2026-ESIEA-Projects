#include "movelist.h"

void movelist_clear(MoveList *list)
{
    list->count = 0;
}

void movelist_add(MoveList *list,
                  Move move)
{
    CHESS_ASSERT(list->count < MAX_MOVES);

    list->moves[list->count++] = move;
}

int movelist_count(const MoveList *list)
{
    return list->count;
}

const Move *movelist_get(const MoveList *list,
                   int index)
{
    CHESS_ASSERT(index >= 0);
    CHESS_ASSERT(index < list->count);

    return &list->moves[index];
}

int movelist_find(const MoveList *list,
                  Move move)
{
    for (int i = 0; i < list->count; i++)
    {
        Move current = list->moves[i];

        if (current.from == move.from &&
            current.to == move.to &&
            current.promotion == move.promotion &&
            current.flags == move.flags)
        {
            return i;
        }
    }

    return -1;
}
