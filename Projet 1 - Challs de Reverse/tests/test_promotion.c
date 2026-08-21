#include <stdio.h>

#include "board/board.h"
#include "coord/coord.h"
#include "move/legalmove.h"
#include "move/makemove.h"
#include "move/move.h"
#include "move/movegen.h"
#include "move/movelist.h"
#include "piece/piece.h"
#include "test.h"


static int has_promotion_move(const MoveList *list,
                              int from,
                              int to,
                              PromotionPiece promotion)
{
    for (int i = 0; i < movelist_count(list); i++)
    {
        const Move *move = movelist_get(list, i);

        if (move->from == from &&
            move->to == to &&
            move->promotion == promotion &&
            move_is_promotion(move))
        {
            return 1;
        }
    }

    return 0;
}


static const Move *find_promotion_move(const MoveList *list,
                                       int from,
                                       int to,
                                       PromotionPiece promotion)
{
    for (int i = 0; i < movelist_count(list); i++)
    {
        const Move *move = movelist_get(list, i);

        if (move->from == from &&
            move->to == to &&
            move->promotion == promotion &&
            move_is_promotion(move))
        {
            return move;
        }
    }

    return NULL;
}


void test_promotion(void)
{
    test_suite("Promotion Tests");

    Board board;
    MoveList list;
    Undo undo;


    /*
     * ========================================
     * PROMOTION BLANCHE SANS CAPTURE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e7"),
        WHITE_PAWN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(4, movelist_count(&list));

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_QUEEN
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_ROOK
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_BISHOP
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_KNIGHT
        )
    );


    /*
     * ========================================
     * PROMOTION NOIRE SANS CAPTURE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e2"),
        BLACK_PAWN
    );

    board.side_to_move = COLOR_BLACK;

    movegen_generate(&board, &list);

    ASSERT_INT(4, movelist_count(&list));

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e1"),
            PROMOTION_QUEEN
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e1"),
            PROMOTION_ROOK
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e1"),
            PROMOTION_BISHOP
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e1"),
            PROMOTION_KNIGHT
        )
    );


    /*
     * ========================================
     * PROMOTION BLANCHE AVEC CAPTURE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e7"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("f8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("f8"),
            PROMOTION_QUEEN
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("f8"),
            PROMOTION_ROOK
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("f8"),
            PROMOTION_BISHOP
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("f8"),
            PROMOTION_KNIGHT
        )
    );


    /*
     * Vérifie que la promotion avec capture
     * possède bien les deux flags.
     */

    const Move *capture_promotion =
        find_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("f8"),
            PROMOTION_QUEEN
        );

    ASSERT_TRUE(capture_promotion != NULL);

    if (capture_promotion != NULL)
    {
        ASSERT_TRUE(
            move_is_capture(capture_promotion)
        );

        ASSERT_TRUE(
            move_is_promotion(capture_promotion)
        );
    }


    /*
     * ========================================
     * MAKE MOVE : PROMOTION EN DAME
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e7"),
        WHITE_PAWN
    );

    board.side_to_move = COLOR_WHITE;

    Move promotion =
        move_create(
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_QUEEN,
            MOVE_PROMOTION
        );

    make_move(
        &board,
        &promotion,
        &undo
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e7")
        )
    );

    ASSERT_INT(
        WHITE_QUEEN,
        board_get_piece(
            &board,
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        WHITE_PAWN,
        undo.moved_piece
    );

    ASSERT_INT(
        COLOR_BLACK,
        board.side_to_move
    );


    /*
     * ========================================
     * UNDO PROMOTION
     * ========================================
     */

    undo_move(
        &board,
        &promotion,
        &undo
    );

    ASSERT_INT(
        WHITE_PAWN,
        board_get_piece(
            &board,
            coord_from_string("e7")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        COLOR_WHITE,
        board.side_to_move
    );


    /*
     * ========================================
     * MAKE MOVE : LES 4 PROMOTIONS
     * ========================================
     */

    PromotionPiece promotions[] =
    {
        PROMOTION_QUEEN,
        PROMOTION_ROOK,
        PROMOTION_BISHOP,
        PROMOTION_KNIGHT
    };

    Piece expected_white[] =
    {
        WHITE_QUEEN,
        WHITE_ROOK,
        WHITE_BISHOP,
        WHITE_KNIGHT
    };

    for (int i = 0; i < 4; i++)
    {
        board_clear(&board);

        board_set_piece(
            &board,
            coord_from_string("e7"),
            WHITE_PAWN
        );

        board.side_to_move = COLOR_WHITE;

        Move move =
            move_create(
                coord_from_string("e7"),
                coord_from_string("e8"),
                promotions[i],
                MOVE_PROMOTION
            );

        make_move(
            &board,
            &move,
            &undo
        );

        ASSERT_INT(
            expected_white[i],
            board_get_piece(
                &board,
                coord_from_string("e8")
            )
        );

        undo_move(
            &board,
            &move,
            &undo
        );

        ASSERT_INT(
            WHITE_PAWN,
            board_get_piece(
                &board,
                coord_from_string("e7")
            )
        );
    }


    /*
     * ========================================
     * PROMOTION NOIRE
     * ========================================
     */

    Piece expected_black[] =
    {
        BLACK_QUEEN,
        BLACK_ROOK,
        BLACK_BISHOP,
        BLACK_KNIGHT
    };

    for (int i = 0; i < 4; i++)
    {
        board_clear(&board);

        board_set_piece(
            &board,
            coord_from_string("e2"),
            BLACK_PAWN
        );

        board.side_to_move = COLOR_BLACK;

        Move move =
            move_create(
                coord_from_string("e2"),
                coord_from_string("e1"),
                promotions[i],
                MOVE_PROMOTION
            );

        make_move(
            &board,
            &move,
            &undo
        );

        ASSERT_INT(
            expected_black[i],
            board_get_piece(
                &board,
                coord_from_string("e1")
            )
        );

        undo_move(
            &board,
            &move,
            &undo
        );

        ASSERT_INT(
            BLACK_PAWN,
            board_get_piece(
                &board,
                coord_from_string("e2")
            )
        );
    }


    /*
     * ========================================
     * PROMOTION AVEC CAPTURE + UNDO
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e7"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("f8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    Move capture_move =
        move_create(
            coord_from_string("e7"),
            coord_from_string("f8"),
            PROMOTION_QUEEN,
            MOVE_CAPTURE | MOVE_PROMOTION
        );

    make_move(
        &board,
        &capture_move,
        &undo
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e7")
        )
    );

    ASSERT_INT(
        WHITE_QUEEN,
        board_get_piece(
            &board,
            coord_from_string("f8")
        )
    );

    ASSERT_INT(
        BLACK_ROOK,
        undo.captured_piece
    );

    undo_move(
        &board,
        &capture_move,
        &undo
    );

    ASSERT_INT(
        WHITE_PAWN,
        board_get_piece(
            &board,
            coord_from_string("e7")
        )
    );

    ASSERT_INT(
        BLACK_ROOK,
        board_get_piece(
            &board,
            coord_from_string("f8")
        )
    );


    /*
     * ========================================
     * LEGALMOVE + PROMOTION
     * ========================================
     *
     * Vérifie que legalmove_generate conserve
     * bien les 4 promotions.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("e7"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    legalmove_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_QUEEN
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_ROOK
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_BISHOP
        )
    );

    ASSERT_TRUE(
        has_promotion_move(
            &list,
            coord_from_string("e7"),
            coord_from_string("e8"),
            PROMOTION_KNIGHT
        )
    );
}
