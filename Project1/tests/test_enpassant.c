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


static const Move *find_enpassant_move(const MoveList *list,
                                       int from,
                                       int to)
{
    for (int i = 0; i < movelist_count(list); i++)
    {
        const Move *move = movelist_get(list, i);

        if (move->from == from &&
            move->to == to &&
            move_is_enpassant(move))
        {
            return move;
        }
    }

    return NULL;
}


void test_enpassant(void)
{
    test_suite("En Passant Tests");

    Board board;
    MoveList list;
    Undo undo;


    /*
     * ========================================
     * DOUBLE AVANCE BLANCHE
     * ========================================
     *
     * e2 -> e4 doit créer e3 comme case
     * de prise en passant.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e2"),
        WHITE_PAWN
    );

    board.side_to_move = COLOR_WHITE;

    Move white_double =
        move_create(
            coord_from_string("e2"),
            coord_from_string("e4"),
            PROMOTION_NONE,
            MOVE_NONE
        );

    make_move(
        &board,
        &white_double,
        &undo
    );

    ASSERT_INT(
        coord_from_string("e3"),
        board.en_passant_square
    );

    undo_move(
        &board,
        &white_double,
        &undo
    );

    ASSERT_INT(
        -1,
        board.en_passant_square
    );


    /*
     * ========================================
     * DOUBLE AVANCE NOIRE
     * ========================================
     *
     * d7 -> d5 doit créer d6.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d7"),
        BLACK_PAWN
    );

    board.side_to_move = COLOR_BLACK;

    Move black_double =
        move_create(
            coord_from_string("d7"),
            coord_from_string("d5"),
            PROMOTION_NONE,
            MOVE_NONE
        );

    make_move(
        &board,
        &black_double,
        &undo
    );

    ASSERT_INT(
        coord_from_string("d6"),
        board.en_passant_square
    );

    undo_move(
        &board,
        &black_double,
        &undo
    );


    /*
     * ========================================
     * GENERATION EN PASSANT BLANCHE
     * ========================================
     *
     * Blanc : pion e5
     * Noir  : pion d5
     *
     * en_passant_square = d6
     *
     * Le blanc doit générer :
     *
     * e5 -> d6
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_PAWN
    );

    board.en_passant_square =
        coord_from_string("d6");

    board.side_to_move = COLOR_WHITE;

    movegen_generate(
        &board,
        &list
    );

    const Move *white_ep =
        find_enpassant_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d6")
        );

    ASSERT_TRUE(
        white_ep != NULL
    );

    if (white_ep != NULL)
    {
        ASSERT_TRUE(
            move_is_enpassant(white_ep)
        );

        ASSERT_TRUE(
            move_is_capture(white_ep)
        );
    }


    /*
     * ========================================
     * MAKE MOVE EN PASSANT BLANC
     * ========================================
     */

    Move white_ep_move =
        move_create(
            coord_from_string("e5"),
            coord_from_string("d6"),
            PROMOTION_NONE,
            MOVE_CAPTURE | MOVE_ENPASSANT
        );

    make_move(
        &board,
        &white_ep_move,
        &undo
    );


    /*
     * Le pion blanc doit être en d6.
     */
    ASSERT_INT(
        WHITE_PAWN,
        board_get_piece(
            &board,
            coord_from_string("d6")
        )
    );


    /*
     * e5 doit être vide.
     */
    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e5")
        )
    );


    /*
     * Le pion noir capturé en d5 doit disparaître.
     */
    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("d5")
        )
    );


    /*
     * Undo doit avoir mémorisé le vrai pion capturé.
     */
    ASSERT_INT(
        BLACK_PAWN,
        undo.captured_piece
    );

    ASSERT_INT(
        coord_from_string("d5"),
        undo.captured_square
    );


    /*
     * Le droit en passant disparaît après le coup.
     */
    ASSERT_INT(
        -1,
        board.en_passant_square
    );


    /*
     * ========================================
     * UNDO EN PASSANT BLANC
     * ========================================
     */

    undo_move(
        &board,
        &white_ep_move,
        &undo
    );

    ASSERT_INT(
        WHITE_PAWN,
        board_get_piece(
            &board,
            coord_from_string("e5")
        )
    );

    ASSERT_INT(
        BLACK_PAWN,
        board_get_piece(
            &board,
            coord_from_string("d5")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("d6")
        )
    );

    ASSERT_INT(
        coord_from_string("d6"),
        board.en_passant_square
    );


    /*
     * ========================================
     * GENERATION EN PASSANT NOIRE
     * ========================================
     *
     * Noir  : pion e4
     * Blanc : pion d4
     *
     * en_passant_square = d3
     *
     * Noir peut jouer :
     *
     * e4 -> d3
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        BLACK_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d4"),
        WHITE_PAWN
    );

    board.en_passant_square =
        coord_from_string("d3");

    board.side_to_move = COLOR_BLACK;

    movegen_generate(
        &board,
        &list
    );

    const Move *black_ep =
        find_enpassant_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d3")
        );

    ASSERT_TRUE(
        black_ep != NULL
    );

    if (black_ep != NULL)
    {
        ASSERT_TRUE(
            move_is_enpassant(black_ep)
        );

        ASSERT_TRUE(
            move_is_capture(black_ep)
        );
    }


    /*
     * ========================================
     * MAKE + UNDO EN PASSANT NOIR
     * ========================================
     */

    Move black_ep_move =
        move_create(
            coord_from_string("e4"),
            coord_from_string("d3"),
            PROMOTION_NONE,
            MOVE_CAPTURE | MOVE_ENPASSANT
        );

    make_move(
        &board,
        &black_ep_move,
        &undo
    );

    ASSERT_INT(
        BLACK_PAWN,
        board_get_piece(
            &board,
            coord_from_string("d3")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("d4")
        )
    );

    ASSERT_INT(
        WHITE_PAWN,
        undo.captured_piece
    );

    ASSERT_INT(
        coord_from_string("d4"),
        undo.captured_square
    );

    undo_move(
        &board,
        &black_ep_move,
        &undo
    );

    ASSERT_INT(
        BLACK_PAWN,
        board_get_piece(
            &board,
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        WHITE_PAWN,
        board_get_piece(
            &board,
            coord_from_string("d4")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("d3")
        )
    );


    /*
     * ========================================
     * PAS D'EN PASSANT SANS DROIT
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_PAWN
    );

    board.en_passant_square = -1;
    board.side_to_move = COLOR_WHITE;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_enpassant_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d6")
        ) == NULL
    );


    /*
     * ========================================
     * FAUSSE CASE EN PASSANT
     * ========================================
     *
     * Même si d6 est renseignée comme case
     * en passant, il doit réellement y avoir
     * un pion adverse en d5.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_ROOK
    );

    board.en_passant_square =
        coord_from_string("d6");

    board.side_to_move = COLOR_WHITE;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_enpassant_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d6")
        ) == NULL
    );


    /*
     * ========================================
     * LE DROIT EXPIRE APRES UN AUTRE COUP
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("g1"),
        WHITE_KNIGHT
    );

    board.en_passant_square =
        coord_from_string("d6");

    board.side_to_move = COLOR_WHITE;

    Move other_move =
        move_create(
            coord_from_string("g1"),
            coord_from_string("f3"),
            PROMOTION_NONE,
            MOVE_NONE
        );

    make_move(
        &board,
        &other_move,
        &undo
    );

    ASSERT_INT(
        -1,
        board.en_passant_square
    );

    undo_move(
        &board,
        &other_move,
        &undo
    );

    ASSERT_INT(
        coord_from_string("d6"),
        board.en_passant_square
    );


    /*
     * ========================================
     * LEGALMOVE + EN PASSANT
     * ========================================
     *
     * Vérifie que la prise en passant traverse
     * correctement legalmove_generate().
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("e5"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board.en_passant_square =
        coord_from_string("d6");

    board.side_to_move = COLOR_WHITE;

    legalmove_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_enpassant_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d6")
        ) != NULL
    );
}
