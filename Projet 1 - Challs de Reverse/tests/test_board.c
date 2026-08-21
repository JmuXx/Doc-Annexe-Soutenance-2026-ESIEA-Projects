#include "board/board.h"
#include "coord/coord.h"
#include "piece/piece.h"
#include "test.h"


void test_board(void)
{
    test_suite("Board Tests");


    Board board;


    /*
     * ========================================
     * POSITION INITIALE
     * ========================================
     */

    board_init(&board);

    ASSERT_INT(
        coord_from_string("e1"),
        board_find_king(&board, COLOR_WHITE)
    );

    ASSERT_INT(
        coord_from_string("e8"),
        board_find_king(&board, COLOR_BLACK)
    );


    /*
     * ========================================
     * ROI BLANC À UNE AUTRE POSITION
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("c4"),
        WHITE_KING
    );

    ASSERT_INT(
        coord_from_string("c4"),
        board_find_king(&board, COLOR_WHITE)
    );


    /*
     * ========================================
     * ROI NOIR À UNE AUTRE POSITION
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("g6"),
        BLACK_KING
    );

    ASSERT_INT(
        coord_from_string("g6"),
        board_find_king(&board, COLOR_BLACK)
    );


    /*
     * ========================================
     * LES DEUX ROIS
     * ========================================
     *
     * Vérifie que la couleur demandée est bien
     * prise en compte.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("b2"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("f7"),
        BLACK_KING
    );

    ASSERT_INT(
        coord_from_string("b2"),
        board_find_king(&board, COLOR_WHITE)
    );

    ASSERT_INT(
        coord_from_string("f7"),
        board_find_king(&board, COLOR_BLACK)
    );


    /*
     * ========================================
     * ROI BLANC ABSENT
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_KING
    );

    ASSERT_INT(
        -1,
        board_find_king(&board, COLOR_WHITE)
    );


    /*
     * ========================================
     * ROI NOIR ABSENT
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    ASSERT_INT(
        -1,
        board_find_king(&board, COLOR_BLACK)
    );


    /*
     * ========================================
     * PLATEAU VIDE
     * ========================================
     */

    board_clear(&board);

    ASSERT_INT(
        -1,
        board_find_king(&board, COLOR_WHITE)
    );

    ASSERT_INT(
        -1,
        board_find_king(&board, COLOR_BLACK)
    );


    /*
     * ========================================
     * COULEUR INVALIDE
     * ========================================
     */

    board_init(&board);

    ASSERT_INT(
        -1,
        board_find_king(&board, COLOR_NONE)
    );

        /*
     * ========================================
     * CHECKMATE
     * ========================================
     *
     * Position :
     *
     * Roi noir : h8
     * Dame blanche : g7
     * Roi blanc : f6
     *
     * Le roi noir est en échec et
     * ne possède aucun coup légal.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board_set_piece(
        &board,
        coord_from_string("g7"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("f6"),
        WHITE_KING
    );

    board.side_to_move = COLOR_BLACK;

    ASSERT_TRUE(
        board_is_checkmate(&board)
    );

    ASSERT_FALSE(
        board_is_stalemate(&board)
    );


    /*
     * ========================================
     * PAS MAT SI LE ROI PEUT S'ECHAPPER
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board_set_piece(
        &board,
        coord_from_string("g6"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("f6"),
        WHITE_KING
    );

    board.side_to_move = COLOR_BLACK;

    ASSERT_FALSE(
        board_is_checkmate(&board)
    );


    /*
     * ========================================
     * STALEMATE / PAT
     * ========================================
     *
     * Position classique :
     *
     * Roi noir : h8
     * Roi blanc : f7
     * Dame blanche : g6
     *
     * Le roi noir n'est pas en échec,
     * mais il ne possède aucun coup légal.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board_set_piece(
        &board,
        coord_from_string("f7"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("g6"),
        WHITE_QUEEN
    );

    board.side_to_move = COLOR_BLACK;

    ASSERT_TRUE(
        board_is_stalemate(&board)
    );

    ASSERT_FALSE(
        board_is_checkmate(&board)
    );


    /*
     * ========================================
     * PAS PAT SI UN COUP LEGAL EXISTE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_KING
    );

    board.side_to_move = COLOR_BLACK;

    ASSERT_FALSE(
        board_is_stalemate(&board)
    );

    ASSERT_FALSE(
        board_is_checkmate(&board)
    );


    /*
     * ========================================
     * POSITION INITIALE
     * ========================================
     *
     * Evidemment ni mat ni pat.
     */

    board_init(&board);

    ASSERT_FALSE(
        board_is_checkmate(&board)
    );

    ASSERT_FALSE(
        board_is_stalemate(&board)
    );
}
