#include "move.h"

Move move_create(int from,
                 int to,
                 PromotionPiece promotion,
                 unsigned int flags)
{
    Move move;

    move.from = from;
    move.to = to;

    move.promotion = promotion;

    move.flags = flags;

    return move;
}

int move_is_capture(const Move *move)
{
    return (move->flags & MOVE_CAPTURE) != 0;
}

int move_is_promotion(const Move *move)
{
    return (move->flags & MOVE_PROMOTION) != 0;
}

int move_is_enpassant(const Move *move)
{
    return (move->flags & MOVE_ENPASSANT) != 0;
}

int move_is_castle(const Move *move)
{
    return (move->flags & MOVE_CASTLE) != 0;
}

