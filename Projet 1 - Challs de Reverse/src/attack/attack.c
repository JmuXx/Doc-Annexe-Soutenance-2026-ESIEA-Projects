#include "attack/attack.h"


static int attacked_by_pawn(const Board *board,
                            int square,
                            PieceColor attacker)
{
    int source1;
    int source2;
    Piece expected_pawn;

    if (attacker == COLOR_WHITE)
    {
        source1 = square - 15;
        source2 = square - 17;

        expected_pawn = WHITE_PAWN;
    }
    else
    {

        source1 = square + 15;
        source2 = square + 17;

        expected_pawn = BLACK_PAWN;
    }

    if (!(source1 & 0x88) &&
        board_get_piece(board, source1) == expected_pawn)
    {
        return 1;
    }

    if (!(source2 & 0x88) &&
        board_get_piece(board, source2) == expected_pawn)
    {
        return 1;
    }

    return 0;
}


static int attacked_by_knight(const Board *board,
                              int square,
                              PieceColor attacker)
{
    static const int offsets[] =
    {
        31,
        33,
        14,
        18,
        -31,
        -33,
        -14,
        -18
    };

    Piece expected_knight =
        attacker == COLOR_WHITE
        ? WHITE_KNIGHT
        : BLACK_KNIGHT;

    for (int i = 0; i < 8; i++)
    {
        int source = square + offsets[i];

        if (source & 0x88)
        {
            continue;
        }

        if (board_get_piece(board, source) == expected_knight)
        {
            return 1;
        }
    }

    return 0;
}


static int attacked_diagonally(const Board *board,
                               int square,
                               PieceColor attacker)
{
    static const int directions[] =
    {
        15,
        17,
        -15,
        -17
    };

    for (int i = 0; i < 4; i++)
    {
        int source = square + directions[i];

        while (!(source & 0x88))
        {
            Piece piece = board_get_piece(board, source);

            if (piece == EMPTY)
            {
                source += directions[i];
                continue;
            }

            if (piece_color(piece) == attacker)
            {
                if (piece_is_bishop(piece) ||
                    piece_is_queen(piece))
                {
                    return 1;
                }
            }

            break;
        }
    }

    return 0;
}

static int attacked_straight(const Board *board,
                             int square,
                             PieceColor attacker)
{
    static const int directions[] =
    {
        16,
        -16,
        1,
        -1
    };

    for (int i = 0; i < 4; i++)
    {
        int source = square + directions[i];

        while (!(source & 0x88))
        {
            Piece piece = board_get_piece(board, source);

            if (piece == EMPTY)
            {
                source += directions[i];
                continue;
            }

            if (piece_color(piece) == attacker)
            {
                if (piece_is_rook(piece) ||
                    piece_is_queen(piece))
                {
                    return 1;
                }
            }

            break;
        }
    }

    return 0;
}


static int attacked_by_king(const Board *board,
                            int square,
                            PieceColor attacker)
{
    static const int offsets[] =
    {
        16,
        -16,
        1,
        -1,
        15,
        17,
        -15,
        -17
    };

    Piece expected_king =
        attacker == COLOR_WHITE
        ? WHITE_KING
        : BLACK_KING;

    for (int i = 0; i < 8; i++)
    {
        int source = square + offsets[i];

        if (source & 0x88)
        {
            continue;
        }

        if (board_get_piece(board, source) == expected_king)
        {
            return 1;
        }
    }

    return 0;
}


int board_is_square_attacked(const Board *board,
                             int square,
                             PieceColor attacker)
{
    CHESS_ASSERT(board != NULL);
    CHESS_ASSERT(!(square & 0x88));
    CHESS_ASSERT(
        attacker == COLOR_WHITE ||
        attacker == COLOR_BLACK
    );

    if (attacked_by_pawn(board, square, attacker))
    {
        return 1;
    }

    if (attacked_by_knight(board, square, attacker))
    {
        return 1;
    }

    if (attacked_diagonally(board, square, attacker))
    {
        return 1;
    }

    if (attacked_straight(board, square, attacker))
    {
        return 1;
    }

    if (attacked_by_king(board, square, attacker))
    {
        return 1;
    }

    return 0;
}

int board_is_in_check(const Board *board,
                      PieceColor color)
{
    int king_square = board_find_king(board, color);

    if (king_square == -1)
    {
        return 0;
    }

    PieceColor opponent =
        color == COLOR_WHITE
        ? COLOR_BLACK
        : COLOR_WHITE;

    return board_is_square_attacked(
        board,
        king_square,
        opponent
    );
}