#include <stdio.h>

#include "move/makemove.h"
#include "coord/coord.h"
#include "piece/piece.h"


static Piece promotion_to_piece(PromotionPiece promotion,
                                PieceColor color)
{
    if (color == COLOR_WHITE)
    {
        switch (promotion)
        {
            case PROMOTION_KNIGHT:
                return WHITE_KNIGHT;

            case PROMOTION_BISHOP:
                return WHITE_BISHOP;

            case PROMOTION_ROOK:
                return WHITE_ROOK;

            case PROMOTION_QUEEN:
                return WHITE_QUEEN;

            default:
                return EMPTY;
        }
    }

    if (color == COLOR_BLACK)
    {
        switch (promotion)
        {
            case PROMOTION_KNIGHT:
                return BLACK_KNIGHT;

            case PROMOTION_BISHOP:
                return BLACK_BISHOP;

            case PROMOTION_ROOK:
                return BLACK_ROOK;

            case PROMOTION_QUEEN:
                return BLACK_QUEEN;

            default:
                return EMPTY;
        }
    }

    return EMPTY;
}


static PieceColor opposite_color(PieceColor color)
{
    if (color == COLOR_WHITE)
    {
        return COLOR_BLACK;
    }

    if (color == COLOR_BLACK)
    {
        return COLOR_WHITE;
    }

    return COLOR_NONE;
}

static void make_castle_rook_move(Board *board,
                                  const Move *move)
{
    int rook_from = -1;
    int rook_to = -1;


    if (move->from == coord_from_string("e1") &&
        move->to == coord_from_string("g1"))
    {
        rook_from = coord_from_string("h1");
        rook_to = coord_from_string("f1");
    }


    else if (move->from == coord_from_string("e1") &&
             move->to == coord_from_string("c1"))
    {
        rook_from = coord_from_string("a1");
        rook_to = coord_from_string("d1");
    }

    else if (move->from == coord_from_string("e8") &&
             move->to == coord_from_string("g8"))
    {
        rook_from = coord_from_string("h8");
        rook_to = coord_from_string("f8");
    }

    else if (move->from == coord_from_string("e8") &&
             move->to == coord_from_string("c8"))
    {
        rook_from = coord_from_string("a8");
        rook_to = coord_from_string("d8");
    }

    CHESS_ASSERT(rook_from != -1);
    CHESS_ASSERT(rook_to != -1);

    Piece rook =
        board_get_piece(
            board,
            rook_from
        );

    CHESS_ASSERT(
        piece_is_rook(rook)
    );

    board_set_piece(
        board,
        rook_to,
        rook
    );

    board_set_piece(
        board,
        rook_from,
        EMPTY
    );
}


static void undo_castle_rook_move(Board *board,
                                  const Move *move)
{
    int rook_from = -1;
    int rook_to = -1;

    if (move->from == coord_from_string("e1") &&
        move->to == coord_from_string("g1"))
    {
        rook_from = coord_from_string("f1");
        rook_to = coord_from_string("h1");
    }

    else if (move->from == coord_from_string("e1") &&
             move->to == coord_from_string("c1"))
    {
        rook_from = coord_from_string("d1");
        rook_to = coord_from_string("a1");
    }

    else if (move->from == coord_from_string("e8") &&
             move->to == coord_from_string("g8"))
    {
        rook_from = coord_from_string("f8");
        rook_to = coord_from_string("h8");
    }

    else if (move->from == coord_from_string("e8") &&
             move->to == coord_from_string("c8"))
    {
        rook_from = coord_from_string("d8");
        rook_to = coord_from_string("a8");
    }

    CHESS_ASSERT(rook_from != -1);
    CHESS_ASSERT(rook_to != -1);

    Piece rook =
        board_get_piece(
            board,
            rook_from
        );

    CHESS_ASSERT(
        piece_is_rook(rook)
    );

    board_set_piece(
        board,
        rook_to,
        rook
    );

    board_set_piece(
        board,
        rook_from,
        EMPTY
    );
}


static void update_castling_rights(Board *board,
                                   Piece moving_piece,
                                   int from,
                                   Piece captured_piece,
                                   int captured_square)
{
    
    if (moving_piece == WHITE_KING)
    {
        board->castling_rights &=
            ~(
                CASTLE_WHITE_KINGSIDE |
                CASTLE_WHITE_QUEENSIDE
            );
    }


    if (moving_piece == BLACK_KING)
    {
        board->castling_rights &=
            ~(
                CASTLE_BLACK_KINGSIDE |
                CASTLE_BLACK_QUEENSIDE
            );
    }


    if (moving_piece == WHITE_ROOK)
    {
        
        if (from == coord_from_string("h1"))
        {
            board->castling_rights &=
                ~CASTLE_WHITE_KINGSIDE;
        }

        if (from == coord_from_string("a1"))
        {
            board->castling_rights &=
                ~CASTLE_WHITE_QUEENSIDE;
        }
    }


    if (moving_piece == BLACK_ROOK)
    {
        if (from == coord_from_string("h8"))
        {
            board->castling_rights &=
                ~CASTLE_BLACK_KINGSIDE;
        }

        if (from == coord_from_string("a8"))
        {
            board->castling_rights &=
                ~CASTLE_BLACK_QUEENSIDE;
        }
    }

    if (captured_piece == WHITE_ROOK)
    {
        if (captured_square ==
            coord_from_string("h1"))
        {
            board->castling_rights &=
                ~CASTLE_WHITE_KINGSIDE;
        }

        if (captured_square ==
            coord_from_string("a1"))
        {
            board->castling_rights &=
                ~CASTLE_WHITE_QUEENSIDE;
        }
    }

    if (captured_piece == BLACK_ROOK)
    {
        if (captured_square ==
            coord_from_string("h8"))
        {
            board->castling_rights &=
                ~CASTLE_BLACK_KINGSIDE;
        }

        if (captured_square ==
            coord_from_string("a8"))
        {
            board->castling_rights &=
                ~CASTLE_BLACK_QUEENSIDE;
        }
    }
}


void make_move(Board *board,
               const Move *move,
               Undo *undo)
{
    CHESS_ASSERT(board != NULL);
    CHESS_ASSERT(move != NULL);
    CHESS_ASSERT(undo != NULL);

    CHESS_ASSERT(
        coord_is_valid(move->from)
    );

    CHESS_ASSERT(
        coord_is_valid(move->to)
    );

    Piece moving_piece =
        board_get_piece(
            board,
            move->from
        );

    CHESS_ASSERT(
        moving_piece != EMPTY
    );

    int captured_square =
        move->to;


    if (move_is_enpassant(move))
    {
        CHESS_ASSERT(
            piece_is_pawn(moving_piece)
        );

        if (piece_is_white(moving_piece))
        {
            captured_square =
                move->to - 16;
        }
        else
        {
            captured_square =
                move->to + 16;
        }
    }

    Piece captured_piece =
        board_get_piece(
            board,
            captured_square
        );

    undo->moved_piece =
        moving_piece;

    undo->captured_piece =
        captured_piece;

    undo->captured_square =
        captured_square;

    undo->castling_rights =
        board->castling_rights;

    undo->en_passant_square =
        board->en_passant_square;

    undo->halfmove_clock =
        board->halfmove_clock;

    undo->fullmove_number =
        board->fullmove_number;

    int is_double_pawn_move =
        piece_is_pawn(moving_piece) &&
        (
            move->to - move->from == 32 ||
            move->from - move->to == 32
        );

    Piece placed_piece =
        moving_piece;

    if (move_is_promotion(move))
    {
        CHESS_ASSERT(
            piece_is_pawn(moving_piece)
        );

        placed_piece =
            promotion_to_piece(
                move->promotion,
                piece_color(moving_piece)
            );

        CHESS_ASSERT(
            placed_piece != EMPTY
        );
    }

    board_set_piece(
        board,
        move->to,
        placed_piece
    );

    board_set_piece(
        board,
        move->from,
        EMPTY
    );

    if (move_is_castle(move))
    {
        CHESS_ASSERT(
            piece_is_king(moving_piece)
        );

        make_castle_rook_move(
            board,
            move
        );
    }

    if (move_is_enpassant(move))
    {
        CHESS_ASSERT(
            captured_piece != EMPTY
        );

        CHESS_ASSERT(
            piece_is_pawn(captured_piece)
        );

        board_set_piece(
            board,
            captured_square,
            EMPTY
        );
    }

    update_castling_rights(
        board,
        moving_piece,
        move->from,
        captured_piece,
        captured_square
    );

    if (piece_is_pawn(moving_piece) ||
        captured_piece != EMPTY)
    {
        board->halfmove_clock = 0;
    }
    else
    {
        board->halfmove_clock++;
    }

    board->en_passant_square = -1;

    if (is_double_pawn_move)
    {
        board->en_passant_square =
            (move->from + move->to) / 2;
    }

    if (board->side_to_move ==
        COLOR_BLACK)
    {
        board->fullmove_number++;
    }

    board->side_to_move =
        opposite_color(
            board->side_to_move
        );
}


void undo_move(Board *board,
               const Move *move,
               const Undo *undo)
{
    CHESS_ASSERT(board != NULL);
    CHESS_ASSERT(move != NULL);
    CHESS_ASSERT(undo != NULL);

    CHESS_ASSERT(
        coord_is_valid(move->from)
    );

    CHESS_ASSERT(
        coord_is_valid(move->to)
    );

    board_set_piece(
        board,
        move->from,
        undo->moved_piece
    );

    board_set_piece(
        board,
        move->to,
        EMPTY
    );

    if (undo->captured_piece != EMPTY)
    {
        board_set_piece(
            board,
            undo->captured_square,
            undo->captured_piece
        );
    }

    if (move_is_castle(move))
    {
        undo_castle_rook_move(
            board,
            move
        );
    }

    board->castling_rights =
        undo->castling_rights;

    board->en_passant_square =
        undo->en_passant_square;

    board->halfmove_clock =
        undo->halfmove_clock;

    board->fullmove_number =
        undo->fullmove_number;

    board->side_to_move =
        opposite_color(
            board->side_to_move
        );
}