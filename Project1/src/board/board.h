#ifndef BOARD_H
#define BOARD_H

#include "defs.h"
#include "piece/piece.h"

enum
{
    CASTLE_WHITE_KINGSIDE  = 1 << 0,
    CASTLE_WHITE_QUEENSIDE = 1 << 1,
    CASTLE_BLACK_KINGSIDE  = 1 << 2,
    CASTLE_BLACK_QUEENSIDE = 1 << 3
};


typedef struct
{
    Piece squares[128];

    PieceColor side_to_move;

    unsigned int castling_rights;

    int en_passant_square;

    int halfmove_clock;

    int fullmove_number;

} Board;


void board_init(Board *board);

void board_print(const Board *board);


Piece board_get_piece(const Board *board,
                      int square);

void board_set_piece(Board *board,
                     int square,
                     Piece piece);

void board_move_piece(Board *board,
                      int from,
                      int to);

void board_clear(Board *board);

int board_find_king(const Board *board,
                    PieceColor color);

int board_is_checkmate(Board *board);

int board_is_stalemate(Board *board);


#endif