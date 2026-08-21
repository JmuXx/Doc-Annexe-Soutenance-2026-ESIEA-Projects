#include <stdio.h>

#include "board/board.h"
#include "coord/coord.h"
#include "piece/piece.h"
#include "attack/attack.h"
#include "move/legalmove.h"

void board_clear(Board *board)
{
    for (int square = 0; square < 128; square++)
    {
        board->squares[square] = EMPTY;
    }

    board->side_to_move = COLOR_WHITE;

    board->castling_rights = 0;

    board->en_passant_square = -1;

    board->halfmove_clock = 0;

    board->fullmove_number = 1;
}

Piece board_get_piece(const Board *board, int square)
{
    return board->squares[square];
}

void board_set_piece(Board *board,
                     int square,
                     Piece piece)
{
    board->squares[square] = piece;
}

void board_move_piece(Board *board,
                      int from,
                      int to)
{
    CHESS_ASSERT(coord_is_valid(from));
    CHESS_ASSERT(coord_is_valid(to));

    Piece piece = board_get_piece(board, from);

    CHESS_ASSERT(piece != EMPTY);

    board_set_piece(board, to, piece);
    board_set_piece(board, from, EMPTY);
}

void board_init(Board *board)
{
    board_clear(board);

    board_set_piece(board, coord_from_string("a1"), WHITE_ROOK);
    board_set_piece(board, coord_from_string("b1"), WHITE_KNIGHT);
    board_set_piece(board, coord_from_string("c1"), WHITE_BISHOP);
    board_set_piece(board, coord_from_string("d1"), WHITE_QUEEN);
    board_set_piece(board, coord_from_string("e1"), WHITE_KING);
    board_set_piece(board, coord_from_string("f1"), WHITE_BISHOP);
    board_set_piece(board, coord_from_string("g1"), WHITE_KNIGHT);
    board_set_piece(board, coord_from_string("h1"), WHITE_ROOK);

    for (char file = 'a'; file <= 'h'; file++)
    {
        char square[3] = { file, '2', '\0' };

        board_set_piece(board,
                        coord_from_string(square),
                        WHITE_PAWN);
    }

    board_set_piece(board, coord_from_string("a8"), BLACK_ROOK);
    board_set_piece(board, coord_from_string("b8"), BLACK_KNIGHT);
    board_set_piece(board, coord_from_string("c8"), BLACK_BISHOP);
    board_set_piece(board, coord_from_string("d8"), BLACK_QUEEN);
    board_set_piece(board, coord_from_string("e8"), BLACK_KING);
    board_set_piece(board, coord_from_string("f8"), BLACK_BISHOP);
    board_set_piece(board, coord_from_string("g8"), BLACK_KNIGHT);
    board_set_piece(board, coord_from_string("h8"), BLACK_ROOK);

    for (char file = 'a'; file <= 'h'; file++)
    {
        char square[3] = { file, '7', '\0' };

        board_set_piece(board,
                        coord_from_string(square),
                        BLACK_PAWN);
    }

    board->side_to_move = COLOR_WHITE;

    board->castling_rights = 0xF;

    board->en_passant_square = -1;

    board->halfmove_clock = 0;

    board->fullmove_number = 1;
}

int board_find_king(const Board *board,
                    PieceColor color)
{
    Piece king;

    if (color == COLOR_WHITE)
    {
        king = WHITE_KING;
    }
    else if (color == COLOR_BLACK)
    {
        king = BLACK_KING;
    }
    else
    {
        return -1;
    }

    for (int square = 0; square < 128; square++)
    {
        if (square & 0x88)
        {
            continue;
        }

        if (board_get_piece(board, square) == king)
        {
            return square;
        }
    }

    return -1;
}

int board_is_checkmate(Board *board)
{
    MoveList legal_moves;

    legalmove_generate(
        board,
        &legal_moves
    );

    return board_is_in_check(
               board,
               board->side_to_move
           ) &&
           movelist_count(&legal_moves) == 0;
}


int board_is_stalemate(Board *board)
{
    MoveList legal_moves;

    legalmove_generate(
        board,
        &legal_moves
    );

    return !board_is_in_check(
                board,
                board->side_to_move
            ) &&
            movelist_count(&legal_moves) == 0;
}
void board_print(const Board *board)
{
    const char *WHITE_COLOR = "\033[1;97m";
    const char *BLACK_COLOR = "\033[0;34m";
    const char *RESET_COLOR = "\033[0m";

    printf("\n");

    printf("    a b c d e f g h\n");

    printf("  +-----------------+\n");

    for (int rank = 7; rank >= 0; rank--)
    {

        printf("%d | ", rank + 1);

        for (int file = 0; file < 8; file++)
        {
            int square =
                rank * 16 + file;

            Piece piece =
                board_get_piece(
                    board,
                    square
                );

            char symbol =
                piece_to_char(piece);

            if (piece_is_white(piece))
            {
                printf(
                    "%s%c%s ",
                    WHITE_COLOR,
                    symbol,
                    RESET_COLOR
                );
            }

            else if (piece_is_black(piece))
            {
                printf(
                    "%s%c%s ",
                    BLACK_COLOR,
                    symbol,
                    RESET_COLOR
                );
            }

            else
            {
                printf("%c ", symbol);
            }
        }

        printf("| %d\n", rank + 1);
    }

    printf("  +-----------------+\n");
    printf("    a b c d e f g h\n\n");
}