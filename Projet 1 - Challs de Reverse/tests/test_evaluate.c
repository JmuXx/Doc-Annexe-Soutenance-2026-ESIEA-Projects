#include "board/board.h"
#include "coord/coord.h"
#include "piece/piece.h"
#include "search/evaluate.h"
#include "test.h"


void test_evaluate(void)
{
    test_suite("Evaluate Tests");

    Board board;


    /*
     * ========================================
     * Plateau vide
     * ========================================
     */

    board_clear(&board);

    ASSERT_INT(
        0,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Position initiale
     * ========================================
     *
     * La position est parfaitement symétrique.
     */

    board_init(&board);

    ASSERT_INT(
        0,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Pion blanc en e4
     * ========================================
     *
     * Matériel   : 100
     * Position   : +20
     * Total      : 120
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    ASSERT_INT(
        120,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Pion noir en e5
     * ========================================
     *
     * Position miroir de e4.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_PAWN
    );

    ASSERT_INT(
        -120,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Cavalier blanc en c3
     * ========================================
     *
     * Matériel   : 320
     * Position   : +15
     * Total      : 335
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("c3"),
        WHITE_KNIGHT
    );

    ASSERT_INT(
        335,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Fou noir en c6
     * ========================================
     *
     * Matériel   : 330
     * Position   : +10
     * Total noir : -340
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("c6"),
        BLACK_BISHOP
    );

    ASSERT_INT(
        -340,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Tour blanche en a1
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_ROOK
    );

    ASSERT_INT(
        500,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Dame noire en d8
     * ========================================
     *
     * Matériel : 900
     * Position : -5
     *
     * Total noir = -(900 - 5)
     *            = -895
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d8"),
        BLACK_QUEEN
    );

    ASSERT_INT(
        -895,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Roi blanc seul en e1
     * ========================================
     *
     * Valeur matérielle = 0
     * PST e1 = 0
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    ASSERT_INT(
        0,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Tours symétriques
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_ROOK
    );

    ASSERT_INT(
        0,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Blanc possède un pion e4 de plus
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    ASSERT_INT(
        120,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Noir possède une tour a8 de plus
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a8"),
        BLACK_ROOK
    );

    ASSERT_INT(
        -500,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Plusieurs pièces blanches
     * ========================================
     *
     * Avec bonus/malus PST :
     *
     * Dame d1      : 900 - 5  = 895
     * Tour a1      : 500
     * Fou c1       : 330 - 10 = 320
     * Cavalier b1  : 320 - 40 = 280
     * Pion e2      : 100 - 20 = 80
     *
     * Total = 2075
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d1"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("c1"),
        WHITE_BISHOP
    );

    board_set_piece(
        &board,
        coord_from_string("b1"),
        WHITE_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("e2"),
        WHITE_PAWN
    );

    ASSERT_INT(
        2075,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Plusieurs pièces noires
     * ========================================
     *
     * Position miroir exacte :
     *
     * Total = -2075
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d8"),
        BLACK_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("a8"),
        BLACK_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("c8"),
        BLACK_BISHOP
    );

    board_set_piece(
        &board,
        coord_from_string("b8"),
        BLACK_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("e7"),
        BLACK_PAWN
    );

    ASSERT_INT(
        -2075,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Position déséquilibrée
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d4"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("a8"),
        BLACK_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("c8"),
        BLACK_BISHOP
    );

    ASSERT_INT(
        205,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * Rois + dame blanche
     * ========================================
     *
     * Roi e1 blanc = 0
     * Roi e8 noir  = 0
     * Dame d1      = 895
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
        BLACK_KING
    );

    board_set_piece(
        &board,
        coord_from_string("d1"),
        WHITE_QUEEN
    );

    ASSERT_INT(
        895,
        evaluate_board(&board)
    );


    /*
     * ========================================
     * PST : cavalier au centre > cavalier coin
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_KNIGHT
    );

    int knight_corner_score =
        evaluate_board(&board);


    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d4"),
        WHITE_KNIGHT
    );

    int knight_center_score =
        evaluate_board(&board);


    ASSERT_TRUE(
        knight_center_score >
        knight_corner_score
    );


    /*
     * ========================================
     * PST : symétrie cavalier blanc / noir
     * ========================================
     *
     * c3 blanc correspond à c6 noir.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("c3"),
        WHITE_KNIGHT
    );

    int white_knight_score =
        evaluate_board(&board);


    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("c6"),
        BLACK_KNIGHT
    );

    int black_knight_score =
        evaluate_board(&board);


    ASSERT_INT(
        white_knight_score,
        -black_knight_score
    );


    /*
     * ========================================
     * PST : pion avancé préféré
     * ========================================
     *
     * On compare le même pion blanc
     * sur e2 et e4.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e2"),
        WHITE_PAWN
    );

    int pawn_e2_score =
        evaluate_board(&board);


    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    int pawn_e4_score =
        evaluate_board(&board);


    ASSERT_TRUE(
        pawn_e4_score >
        pawn_e2_score
    );


    /*
     * ========================================
     * PST : position miroir complète
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d4"),
        WHITE_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("c4"),
        WHITE_BISHOP
    );

    int white_position =
        evaluate_board(&board);


    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("c5"),
        BLACK_BISHOP
    );

    int black_position =
        evaluate_board(&board);


    ASSERT_INT(
        white_position,
        -black_position
    );
}