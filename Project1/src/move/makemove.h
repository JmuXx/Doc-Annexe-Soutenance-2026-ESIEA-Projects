#ifndef MAKEMOVE_H
#define MAKEMOVE_H

#include "board/board.h"
#include "move/move.h"
#include "piece/piece.h"


typedef struct
{
    Piece moved_piece;


    Piece captured_piece;

    int captured_square;

    unsigned int castling_rights;

    int en_passant_square;

    int halfmove_clock;

    int fullmove_number;

} Undo;


void make_move(Board *board,
               const Move *move,
               Undo *undo);


void undo_move(Board *board,
               const Move *move,
               const Undo *undo);

#endif
