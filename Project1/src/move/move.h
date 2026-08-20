#ifndef MOVE_H
#define MOVE_H

typedef enum
{
    PROMOTION_NONE = 0,

    PROMOTION_KNIGHT,

    PROMOTION_BISHOP,

    PROMOTION_ROOK,

    PROMOTION_QUEEN

} PromotionPiece;

typedef struct
{
    int from;
    int to;

    PromotionPiece promotion;

    unsigned int flags;

} Move;

enum
{
    MOVE_NONE        = 0,

    MOVE_CAPTURE     = 1 << 0,

    MOVE_ENPASSANT   = 1 << 1,

    MOVE_CASTLE      = 1 << 2,

    MOVE_PROMOTION   = 1 << 3,

};


Move move_create(int from,
                 int to,
                 PromotionPiece promotion,
                 unsigned int flags);


int move_is_capture(const Move *move);

int move_is_enpassant(const Move *move);

int move_is_promotion(const Move *move);

int move_is_castle(const Move *move);



#endif
