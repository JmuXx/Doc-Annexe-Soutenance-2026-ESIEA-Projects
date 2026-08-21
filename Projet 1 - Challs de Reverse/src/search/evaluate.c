#include <stdio.h>

#include "search/evaluate.h"

#include "piece/piece.h"


#define PAWN_VALUE    100
#define KNIGHT_VALUE  320
#define BISHOP_VALUE  330
#define ROOK_VALUE    500
#define QUEEN_VALUE   900


static const int PAWN_TABLE[64] =
{
     0,  0,  0,  0,  0,  0,  0,  0,

     5, 10, 10,-20,-20, 10, 10,  5,

     5, -5,-10,  0,  0,-10, -5,  5,

     0,  0,  0, 20, 20,  0,  0,  0,

     5,  5, 10, 25, 25, 10,  5,  5,

    10, 10, 20, 30, 30, 20, 10, 10,

    50, 50, 50, 50, 50, 50, 50, 50,

     0,  0,  0,  0,  0,  0,  0,  0
};


static const int KNIGHT_TABLE[64] =
{
    -50,-40,-30,-30,-30,-30,-40,-50,

    -40,-20,  0,  5,  5,  0,-20,-40,

    -30,  5, 15, 20, 20, 15,  5,-30,

    -30, 10, 20, 30, 30, 20, 10,-30,

    -30, 10, 20, 30, 30, 20, 10,-30,

    -30,  5, 15, 20, 20, 15,  5,-30,

    -40,-20,  0,  5,  5,  0,-20,-40,

    -50,-40,-30,-30,-30,-30,-40,-50
};

static const int BISHOP_TABLE[64] =
{
    -20,-10,-10,-10,-10,-10,-10,-20,

    -10,  5,  0,  0,  0,  0,  5,-10,

    -10, 10, 10, 10, 10, 10, 10,-10,

    -10,  0, 10, 15, 15, 10,  0,-10,

    -10,  5,  5, 15, 15,  5,  5,-10,

    -10,  0,  5, 10, 10,  5,  0,-10,

    -10,  0,  0,  0,  0,  0,  0,-10,

    -20,-10,-10,-10,-10,-10,-10,-20
};

static const int ROOK_TABLE[64] =
{
     0,  0,  0,  5,  5,  0,  0,  0,

    -5,  0,  0,  0,  0,  0,  0, -5,

    -5,  0,  0,  0,  0,  0,  0, -5,

    -5,  0,  0,  0,  0,  0,  0, -5,

    -5,  0,  0,  0,  0,  0,  0, -5,

    -5,  0,  0,  0,  0,  0,  0, -5,

     5, 10, 10, 10, 10, 10, 10,  5,

     0,  0,  0,  0,  0,  0,  0,  0
};

static const int QUEEN_TABLE[64] =
{
    -20,-10,-10, -5, -5,-10,-10,-20,

    -10,  0,  5,  0,  0,  0,  0,-10,

    -10,  5,  5,  5,  5,  5,  0,-10,

      0,  0,  5,  5,  5,  5,  0, -5,

     -5,  0,  5,  5,  5,  5,  0, -5,

    -10,  0,  5,  5,  5,  5,  0,-10,

    -10,  0,  0,  0,  0,  0,  0,-10,

    -20,-10,-10, -5, -5,-10,-10,-20
};


static const int KING_TABLE[64] =
{
     20, 30, 10,  0,  0, 10, 30, 20,

     20, 20,  0,  0,  0,  0, 20, 20,

    -10,-20,-20,-20,-20,-20,-20,-10,

    -20,-30,-30,-40,-40,-30,-30,-20,

    -30,-40,-40,-50,-50,-40,-40,-30,

    -30,-40,-40,-50,-50,-40,-40,-30,

    -30,-40,-40,-50,-50,-40,-40,-30,

    -30,-40,-40,-50,-50,-40,-40,-30
};


static int square_to_index(int square)
{
    int rank =
        square >> 4;

    int file =
        square & 7;

    return rank * 8 + file;
}


static int mirror_index(int index)
{
    int rank =
        index / 8;

    int file =
        index % 8;

    int mirrored_rank =
        7 - rank;

    return mirrored_rank * 8 + file;
}

static int piece_square_value(Piece piece,
                              int square)
{
    int index =
        square_to_index(
            square
        );

    if (piece_is_black(piece))
    {
        index =
            mirror_index(
                index
            );
    }

    if (piece_is_pawn(piece))
    {
        return PAWN_TABLE[index];
    }

    if (piece_is_knight(piece))
    {
        return KNIGHT_TABLE[index];
    }

    if (piece_is_bishop(piece))
    {
        return BISHOP_TABLE[index];
    }

    if (piece_is_rook(piece))
    {
        return ROOK_TABLE[index];
    }

    if (piece_is_queen(piece))
    {
        return QUEEN_TABLE[index];
    }

    if (piece_is_king(piece))
    {
        return KING_TABLE[index];
    }


    return 0;
}


static int piece_value(Piece piece)
{
    if (piece_is_pawn(piece))
    {
        return PAWN_VALUE;
    }

    if (piece_is_knight(piece))
    {
        return KNIGHT_VALUE;
    }

    if (piece_is_bishop(piece))
    {
        return BISHOP_VALUE;
    }

    if (piece_is_rook(piece))
    {
        return ROOK_VALUE;
    }

    if (piece_is_queen(piece))
    {
        return QUEEN_VALUE;
    }

    if (piece_is_king(piece))
    {
        return 0;
    }


    return 0;
}


int evaluate_board(const Board *board)
{
    CHESS_ASSERT(
        board != NULL
    );


    int score =
        0;


    for (int square = 0;
         square < 128;
         square++)
    {

        if (square & 0x88)
        {
            continue;
        }

        Piece piece =
            board_get_piece(
                board,
                square
            );

        if (piece == EMPTY)
        {
            continue;
        }

        int material =
            piece_value(
                piece
            );

        int positional =
            piece_square_value(
                piece,
                square
            );
 
        int value =
            material +
            positional;


        if (piece_is_white(piece))
        {
            score +=
                value;
        }

        else if (piece_is_black(piece))
        {
            score -=
                value;
        }
    }


    return score;
}