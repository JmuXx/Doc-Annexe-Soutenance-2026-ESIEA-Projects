#include "movegen.h"

#include "attack/attack.h"
#include "coord/coord.h"
#include "piece/piece.h"


static void add_promotion_moves(MoveList *list,
                                int from,
                                int to,
                                unsigned int flags)
{
    unsigned int promotion_flags =
        flags | MOVE_PROMOTION;

    movelist_add(
        list,
        move_create(
            from,
            to,
            PROMOTION_QUEEN,
            promotion_flags
        )
    );

    movelist_add(
        list,
        move_create(
            from,
            to,
            PROMOTION_ROOK,
            promotion_flags
        )
    );

    movelist_add(
        list,
        move_create(
            from,
            to,
            PROMOTION_BISHOP,
            promotion_flags
        )
    );

    movelist_add(
        list,
        move_create(
            from,
            to,
            PROMOTION_KNIGHT,
            promotion_flags
        )
    );
}


static void generate_pawn_moves(const Board *board,
                                MoveList *list,
                                int square)
{
    Piece pawn = board_get_piece(board, square);

    int direction;
    int starting_rank;
    int promotion_rank;

    int capture_left_offset;
    int capture_right_offset;


    if (piece_is_white(pawn))
    {
        direction = 16;

        starting_rank = 1;
        promotion_rank = 7;

        capture_left_offset = 15;
        capture_right_offset = 17;
    }
    else
    {
        direction = -16;

        starting_rank = 6;
        promotion_rank = 0;

        capture_left_offset = -17;
        capture_right_offset = -15;
    }
    int forward = square + direction;
    if (!(forward & 0x88) &&
        board_get_piece(board, forward) == EMPTY)
    {
        int target_rank = forward >> 4;

        if (target_rank == promotion_rank)
        {
            add_promotion_moves(
                list,
                square,
                forward,
                MOVE_NONE
            );
        }
        else
        {
            movelist_add(
                list,
                move_create(
                    square,
                    forward,
                    PROMOTION_NONE,
                    MOVE_NONE
                )
            );

            int rank = square >> 4;

            if (rank == starting_rank)
            {
                int double_forward =
                    square + 2 * direction;

                if (!(double_forward & 0x88) &&
                    board_get_piece(
                        board,
                        double_forward
                    ) == EMPTY)
                {
                    movelist_add(
                        list,
                        move_create(
                            square,
                            double_forward,
                            PROMOTION_NONE,
                            MOVE_NONE
                        )
                    );
                }
            }
        }
    }

    int capture_left =
        square + capture_left_offset;

    if (!(capture_left & 0x88))
    {
        Piece target =
            board_get_piece(
                board,
                capture_left
            );

        if (target != EMPTY &&
            piece_color(target) != piece_color(pawn))
        {
            int target_rank =
                capture_left >> 4;

            if (target_rank == promotion_rank)
            {
                add_promotion_moves(
                    list,
                    square,
                    capture_left,
                    MOVE_CAPTURE
                );
            }
            else
            {
                movelist_add(
                    list,
                    move_create(
                        square,
                        capture_left,
                        PROMOTION_NONE,
                        MOVE_CAPTURE
                    )
                );
            }
        }

        if (capture_left ==
            board->en_passant_square)
        {
            int captured_square;

            if (piece_is_white(pawn))
            {
                captured_square =
                    capture_left - 16;
            }
            else
            {
                captured_square =
                    capture_left + 16;
            }

            Piece captured =
                board_get_piece(
                    board,
                    captured_square
                );

            if (piece_is_pawn(captured) &&
                piece_color(captured) !=
                    piece_color(pawn))
            {
                movelist_add(
                    list,
                    move_create(
                        square,
                        capture_left,
                        PROMOTION_NONE,
                        MOVE_CAPTURE |
                        MOVE_ENPASSANT
                    )
                );
            }
        }
    }

    int capture_right =
        square + capture_right_offset;

    if (!(capture_right & 0x88))
    {
        Piece target =
            board_get_piece(
                board,
                capture_right
            );

        if (target != EMPTY &&
            piece_color(target) != piece_color(pawn))
        {
            int target_rank =
                capture_right >> 4;

            if (target_rank == promotion_rank)
            {
                add_promotion_moves(
                    list,
                    square,
                    capture_right,
                    MOVE_CAPTURE
                );
            }
            else
            {
                movelist_add(
                    list,
                    move_create(
                        square,
                        capture_right,
                        PROMOTION_NONE,
                        MOVE_CAPTURE
                    )
                );
            }
        }

        if (capture_right ==
            board->en_passant_square)
        {
            int captured_square;

            if (piece_is_white(pawn))
            {
                captured_square =
                    capture_right - 16;
            }
            else
            {
                captured_square =
                    capture_right + 16;
            }

            Piece captured =
                board_get_piece(
                    board,
                    captured_square
                );

            if (piece_is_pawn(captured) &&
                piece_color(captured) !=
                    piece_color(pawn))
            {
                movelist_add(
                    list,
                    move_create(
                        square,
                        capture_right,
                        PROMOTION_NONE,
                        MOVE_CAPTURE |
                        MOVE_ENPASSANT
                    )
                );
            }
        }
    }
}


static void generate_knight_moves(const Board *board,
                                  MoveList *list,
                                  int square)
{
    Piece knight = board_get_piece(board, square);

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

    for (int i = 0; i < 8; i++)
    {
        int target_square = square + offsets[i];

        if (target_square & 0x88)
        {
            continue;
        }

        Piece target =
            board_get_piece(
                board,
                target_square
            );

        if (target != EMPTY &&
            piece_color(target) == piece_color(knight))
        {
            continue;
        }

        unsigned int flags = MOVE_NONE;

        if (target != EMPTY)
        {
            flags |= MOVE_CAPTURE;
        }

        movelist_add(
            list,
            move_create(
                square,
                target_square,
                PROMOTION_NONE,
                flags
            )
        );
    }
}


static void generate_bishop_moves(const Board *board,
                                  MoveList *list,
                                  int square)
{
    Piece bishop =
        board_get_piece(
            board,
            square
        );

    static const int directions[] =
    {
        15,
        17,
        -15,
        -17
    };

    for (int i = 0; i < 4; i++)
    {
        int target_square =
            square + directions[i];

        while (!(target_square & 0x88))
        {
            Piece target =
                board_get_piece(
                    board,
                    target_square
                );
            if (target == EMPTY)
            {
                movelist_add(
                    list,
                    move_create(
                        square,
                        target_square,
                        PROMOTION_NONE,
                        MOVE_NONE
                    )
                );
            }
            else
            {
                if (piece_color(target) !=
                    piece_color(bishop))
                {
                    movelist_add(
                        list,
                        move_create(
                            square,
                            target_square,
                            PROMOTION_NONE,
                            MOVE_CAPTURE
                        )
                    );
                }

                break;
            }

            target_square +=
                directions[i];
        }
    }
}


static void generate_rook_moves(const Board *board,
                                MoveList *list,
                                int square)
{
    Piece rook =
        board_get_piece(
            board,
            square
        );

    static const int directions[] =
    {
        16,
        -16,
        1,
        -1
    };

    for (int i = 0; i < 4; i++)
    {
        int target_square =
            square + directions[i];

        while (!(target_square & 0x88))
        {
            Piece target =
                board_get_piece(
                    board,
                    target_square
                );

            if (target == EMPTY)
            {
                movelist_add(
                    list,
                    move_create(
                        square,
                        target_square,
                        PROMOTION_NONE,
                        MOVE_NONE
                    )
                );
            }
            else
            {
                if (piece_color(target) !=
                    piece_color(rook))
                {
                    movelist_add(
                        list,
                        move_create(
                            square,
                            target_square,
                            PROMOTION_NONE,
                            MOVE_CAPTURE
                        )
                    );
                }
                break;
            }

            target_square +=
                directions[i];
        }
    }
}


static void generate_queen_moves(const Board *board,
                                 MoveList *list,
                                 int square)
{
    Piece queen =
        board_get_piece(
            board,
            square
        );

    static const int directions[] =
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

    for (int i = 0; i < 8; i++)
    {
        int target_square =
            square + directions[i];

        while (!(target_square & 0x88))
        {
            Piece target =
                board_get_piece(
                    board,
                    target_square
                );

            if (target == EMPTY)
            {
                movelist_add(
                    list,
                    move_create(
                        square,
                        target_square,
                        PROMOTION_NONE,
                        MOVE_NONE
                    )
                );
            }
            else
            {
                if (piece_color(target) !=
                    piece_color(queen))
                {
                    movelist_add(
                        list,
                        move_create(
                            square,
                            target_square,
                            PROMOTION_NONE,
                            MOVE_CAPTURE
                        )
                    );
                }

                break;
            }

            target_square +=
                directions[i];
        }
    }
}


static void generate_king_moves(const Board *board,
                                MoveList *list,
                                int square)
{
    Piece king =
        board_get_piece(
            board,
            square
        );

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

    for (int i = 0; i < 8; i++)
    {
        int target_square =
            square + offsets[i];

        if (target_square & 0x88)
        {
            continue;
        }

        Piece target =
            board_get_piece(
                board,
                target_square
            );

        if (target != EMPTY &&
            piece_color(target) ==
                piece_color(king))
        {
            continue;
        }

        unsigned int flags =
            MOVE_NONE;

        if (target != EMPTY)
        {
            flags = MOVE_CAPTURE;
        }

        movelist_add(
            list,
            move_create(
                square,
                target_square,
                PROMOTION_NONE,
                flags
            )
        );
    }
}


static void generate_castling_moves(const Board *board,
                                    MoveList *list,
                                    int square)
{
    Piece king =
        board_get_piece(
            board,
            square
        );

    PieceColor color =
        piece_color(king);


    if (color == COLOR_WHITE)
    {
        int e1 =
            coord_from_string("e1");

        if (square != e1)
        {
            return;
        }

        if (board->castling_rights &
            CASTLE_WHITE_KINGSIDE)
        {
            int f1 =
                coord_from_string("f1");

            int g1 =
                coord_from_string("g1");

            int h1 =
                coord_from_string("h1");


            if (board_get_piece(
                    board,
                    h1
                ) == WHITE_ROOK &&
                board_get_piece(
                    board,
                    f1
                ) == EMPTY &&
                board_get_piece(
                    board,
                    g1
                ) == EMPTY)
            {
                if (!board_is_square_attacked(
                        board,
                        e1,
                        COLOR_BLACK
                    ) &&
                    !board_is_square_attacked(
                        board,
                        f1,
                        COLOR_BLACK
                    ) &&
                    !board_is_square_attacked(
                        board,
                        g1,
                        COLOR_BLACK
                    ))
                {
                    movelist_add(
                        list,
                        move_create(
                            e1,
                            g1,
                            PROMOTION_NONE,
                            MOVE_CASTLE
                        )
                    );
                }
            }
        }


        if (board->castling_rights &
            CASTLE_WHITE_QUEENSIDE)
        {
            int a1 =
                coord_from_string("a1");

            int b1 =
                coord_from_string("b1");

            int c1 =
                coord_from_string("c1");

            int d1 =
                coord_from_string("d1");


            if (board_get_piece(
                    board,
                    a1
                ) == WHITE_ROOK &&
                board_get_piece(
                    board,
                    b1
                ) == EMPTY &&
                board_get_piece(
                    board,
                    c1
                ) == EMPTY &&
                board_get_piece(
                    board,
                    d1
                ) == EMPTY)
            {
                if (!board_is_square_attacked(
                        board,
                        e1,
                        COLOR_BLACK
                    ) &&
                    !board_is_square_attacked(
                        board,
                        d1,
                        COLOR_BLACK
                    ) &&
                    !board_is_square_attacked(
                        board,
                        c1,
                        COLOR_BLACK
                    ))
                {
                    movelist_add(
                        list,
                        move_create(
                            e1,
                            c1,
                            PROMOTION_NONE,
                            MOVE_CASTLE
                        )
                    );
                }
            }
        }

        return;
    }


    if (color == COLOR_BLACK)
    {
        int e8 =
            coord_from_string("e8");

        if (square != e8)
        {
            return;
        }


        if (board->castling_rights &
            CASTLE_BLACK_KINGSIDE)
        {
            int f8 =
                coord_from_string("f8");

            int g8 =
                coord_from_string("g8");

            int h8 =
                coord_from_string("h8");


            if (board_get_piece(
                    board,
                    h8
                ) == BLACK_ROOK &&
                board_get_piece(
                    board,
                    f8
                ) == EMPTY &&
                board_get_piece(
                    board,
                    g8
                ) == EMPTY)
            {
                if (!board_is_square_attacked(
                        board,
                        e8,
                        COLOR_WHITE
                    ) &&
                    !board_is_square_attacked(
                        board,
                        f8,
                        COLOR_WHITE
                    ) &&
                    !board_is_square_attacked(
                        board,
                        g8,
                        COLOR_WHITE
                    ))
                {
                    movelist_add(
                        list,
                        move_create(
                            e8,
                            g8,
                            PROMOTION_NONE,
                            MOVE_CASTLE
                        )
                    );
                }
            }
        }


        if (board->castling_rights &
            CASTLE_BLACK_QUEENSIDE)
        {
            int a8 =
                coord_from_string("a8");

            int b8 =
                coord_from_string("b8");

            int c8 =
                coord_from_string("c8");

            int d8 =
                coord_from_string("d8");


            if (board_get_piece(
                    board,
                    a8
                ) == BLACK_ROOK &&
                board_get_piece(
                    board,
                    b8
                ) == EMPTY &&
                board_get_piece(
                    board,
                    c8
                ) == EMPTY &&
                board_get_piece(
                    board,
                    d8
                ) == EMPTY)
            {
                if (!board_is_square_attacked(
                        board,
                        e8,
                        COLOR_WHITE
                    ) &&
                    !board_is_square_attacked(
                        board,
                        d8,
                        COLOR_WHITE
                    ) &&
                    !board_is_square_attacked(
                        board,
                        c8,
                        COLOR_WHITE
                    ))
                {
                    movelist_add(
                        list,
                        move_create(
                            e8,
                            c8,
                            PROMOTION_NONE,
                            MOVE_CASTLE
                        )
                    );
                }
            }
        }
    }
}


void movegen_generate(const Board *board,
                      MoveList *list)
{
    movelist_clear(list);

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

        if (piece_color(piece) !=
            board->side_to_move)
        {
            continue;
        }


        if (piece_is_pawn(piece))
        {
            generate_pawn_moves(
                board,
                list,
                square
            );
        }
        else if (piece_is_knight(piece))
        {
            generate_knight_moves(
                board,
                list,
                square
            );
        }
        else if (piece_is_bishop(piece))
        {
            generate_bishop_moves(
                board,
                list,
                square
            );
        }
        else if (piece_is_rook(piece))
        {
            generate_rook_moves(
                board,
                list,
                square
            );
        }
        else if (piece_is_queen(piece))
        {
            generate_queen_moves(
                board,
                list,
                square
            );
        }
        else if (piece_is_king(piece))
        {

            generate_king_moves(
                board,
                list,
                square
            );

            generate_castling_moves(
                board,
                list,
                square
            );
        }
    }
}