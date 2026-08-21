#include "attack/attack.h"
#include "board/board.h"
#include "coord/coord.h"
#include "piece/piece.h"
#include "test.h"


void test_attack(void)
{
    test_suite("Attack Tests");

    Board board;


    /*
     * ========================================
     * PION BLANC
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("d5"),
            COLOR_WHITE
        )
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("f5"),
            COLOR_WHITE
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e5"),
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * PION NOIR
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_PAWN
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("d4"),
            COLOR_BLACK
        )
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("f4"),
            COLOR_BLACK
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e4"),
            COLOR_BLACK
        )
    );


    /*
     * ========================================
     * CAVALIER
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KNIGHT
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("c5"),
            COLOR_WHITE
        )
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("f6"),
            COLOR_WHITE
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e5"),
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * FOU
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("c3"),
        WHITE_BISHOP
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("f6"),
            COLOR_WHITE
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("c6"),
            COLOR_WHITE
        )
    );


    /*
     * Pièce qui bloque le fou.
     */
    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_PAWN
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("f6"),
            COLOR_WHITE
        )
    );

    /*
     * Mais e5 est elle-même attaquée par le fou.
     */
    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("e5"),
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * TOUR
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        BLACK_ROOK
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("e1"),
            COLOR_BLACK
        )
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("a4"),
            COLOR_BLACK
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("a1"),
            COLOR_BLACK
        )
    );


    /*
     * Blocage d'une tour.
     */
    board_set_piece(
        &board,
        coord_from_string("e3"),
        WHITE_PAWN
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e1"),
            COLOR_BLACK
        )
    );


    /*
     * ========================================
     * DAME
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_QUEEN
    );

    /*
     * Comme une tour.
     */
    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("e8"),
            COLOR_WHITE
        )
    );

    /*
     * Comme un fou.
     */
    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("a8"),
            COLOR_WHITE
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("b6"),
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * ROI
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        BLACK_KING
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("e5"),
            COLOR_BLACK
        )
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("f5"),
            COLOR_BLACK
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e6"),
            COLOR_BLACK
        )
    );


    /*
     * ========================================
     * COULEUR
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_ROOK
    );

    ASSERT_TRUE(
        board_is_square_attacked(
            &board,
            coord_from_string("e1"),
            COLOR_BLACK
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e1"),
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * AUCUNE ATTAQUE
     * ========================================
     */

    board_clear(&board);

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e4"),
            COLOR_WHITE
        )
    );

    ASSERT_FALSE(
        board_is_square_attacked(
            &board,
            coord_from_string("e4"),
            COLOR_BLACK
        )
    );

        /*
     * ========================================
     * ECHEC - TOUR
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_ROOK
    );

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * ECHEC - FOU
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_KING
    );

    board_set_piece(
        &board,
        coord_from_string("b5"),
        WHITE_BISHOP
    );

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_BLACK
        )
    );


    /*
     * ========================================
     * PAS EN ECHEC
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("a8"),
        BLACK_ROOK
    );

    ASSERT_FALSE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * ATTAQUE BLOQUEE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    ASSERT_FALSE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * ECHEC - CAVALIER
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("c5"),
        BLACK_KNIGHT
    );

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * ECHEC - PION
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_PAWN
    );

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );


    /*
     * ========================================
     * ECHEC - ROI
     * ========================================
     *
     * Les deux rois sont adjacents.
     * Géométriquement, ils s'attaquent.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_KING
    );

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_BLACK
        )
    );


    /*
     * ========================================
     * ROI ABSENT
     * ========================================
     *
     * Notre implémentation actuelle retourne 0
     * si aucun roi de cette couleur n'existe.
     */

    board_clear(&board);

    ASSERT_FALSE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );

    ASSERT_FALSE(
        board_is_in_check(
            &board,
            COLOR_BLACK
        )
    );
}
