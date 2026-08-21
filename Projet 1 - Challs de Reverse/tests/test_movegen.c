#include <stdio.h>

#include "board/board.h"
#include "coord/coord.h"
#include "move/movegen.h"
#include "move/movelist.h"
#include "piece/piece.h"
#include "test.h"


static int has_move(const MoveList *list, int from, int to)
{
    for (int i = 0; i < movelist_count(list); i++)
    {
        const Move *move = movelist_get(list, i);

        if (move->from == from && move->to == to)
        {
            return 1;
        }
    }

    return 0;
}

static int count_moves_from(const MoveList *list, int from)
{
    int count = 0;

    for (int i = 0; i < movelist_count(list); i++)
    {
        const Move *move = movelist_get(list, i);

        if (move->from == from)
        {
            count++;
        }
    }

    return count;
}

void test_movegen(void)
{
    /*
     * ==================================================
     * PIONS
     * ==================================================
     */

    test_suite("Pawn MoveGen Tests");

    Board board;
    MoveList list;


    /*
     * --------------------------------------------------
     * Position initiale
     * --------------------------------------------------
     *
     * 16 coups de pions
     * + 4 coups de cavaliers
     * = 20 coups.
     */

    board_init(&board);

    movegen_generate(&board, &list);

    ASSERT_INT(20, movelist_count(&list));


    /*
     * Doubles avances blanches.
     */

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("a2"),
            coord_from_string("a4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("h2"),
            coord_from_string("h4")
        )
    );


    /*
     * Avances simples blanches.
     */

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("a2"),
            coord_from_string("a3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("h2"),
            coord_from_string("h3")
        )
    );


    /*
     * Les noirs ne doivent pas jouer lorsque
     * les blancs sont au trait.
     */

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("a7"),
            coord_from_string("a6")
        )
    );


    /*
     * --------------------------------------------------
     * Capture blanche
     * --------------------------------------------------
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("f5"),
        BLACK_BISHOP
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(3, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f5")
        )
    );


    /*
     * --------------------------------------------------
     * Capture noire
     * --------------------------------------------------
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d4"),
        WHITE_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("f4"),
        WHITE_BISHOP
    );

    board.side_to_move = COLOR_BLACK;

    movegen_generate(&board, &list);

    ASSERT_INT(3, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("f4")
        )
    );


    /*
     * --------------------------------------------------
     * Pion bloqué
     * --------------------------------------------------
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e2"),
        WHITE_PAWN
    );

    /*
     * On utilise un roi comme bloqueur pour que
     * le bloqueur ne génère pas encore de coups.
     */

    board_set_piece(
        &board,
        coord_from_string("e3"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);


    ASSERT_INT(
    0,
    count_moves_from(
        &list,
        coord_from_string("e2")
        )
    );

    /*
     * --------------------------------------------------
     * Double avance bloquée
     * --------------------------------------------------
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e2"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("e3"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);


    ASSERT_INT(
    0,
    count_moves_from(
        &list,
        coord_from_string("e2")
        )
    );

    /*
     * --------------------------------------------------
     * Pièce alliée en diagonale
     * --------------------------------------------------
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_PAWN
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("f5"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    /*
     * Seul e4 -> e5 est possible.
     */

    ASSERT_INT(
    1,
    count_moves_from(
        &list,
        coord_from_string("e4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d5")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f5")
        )
    );


    /*
     * ==================================================
     * CAVALIERS
     * ==================================================
     */

    test_suite("Knight MoveGen Tests");


    /*
     * --------------------------------------------------
     * Cavalier au centre
     * --------------------------------------------------
     *
     * Un cavalier en e4 possède 8 destinations.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(8, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("c3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("c5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d2")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d6")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f2")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f6")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("g3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("g5")
        )
    );


    /*
     * --------------------------------------------------
     * Cavalier dans un coin
     * --------------------------------------------------
     *
     * Un cavalier en a1 possède seulement 2 coups.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(2, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("a1"),
            coord_from_string("b3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("a1"),
            coord_from_string("c2")
        )
    );


    /*
     * --------------------------------------------------
     * Cavalier bloqué par des pièces alliées
     * --------------------------------------------------
     *
     * Toutes ses destinations sont occupées par
     * des pièces alliées.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("c3"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("c5"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("d2"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("d6"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("f2"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("f6"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("g3"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("g5"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(
    0,
    count_moves_from(
        &list,
        coord_from_string("e4")
        )
    );


    /*
     * --------------------------------------------------
     * Capture avec un cavalier
     * --------------------------------------------------
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KNIGHT
    );

    board_set_piece(
        &board,
        coord_from_string("c5"),
        BLACK_QUEEN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(8, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("c5")
        )
    );

    const Move *capture = NULL;

    for (int i = 0; i < movelist_count(&list); i++)
    {
        const Move *move = movelist_get(&list, i);

        if (move->from == coord_from_string("e4") &&
            move->to == coord_from_string("c5"))
        {
            capture = move;
            break;
        }
    }

    ASSERT_TRUE(capture != NULL);
    ASSERT_TRUE(move_is_capture(capture));


    /*
     * --------------------------------------------------
     * Cavalier noir
     * --------------------------------------------------
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_KNIGHT
    );

    board.side_to_move = COLOR_BLACK;

    movegen_generate(&board, &list);

    ASSERT_INT(8, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("c4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("c6")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d7")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("f3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("f7")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("g4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("g6")
        )
    );
}

void test_bishop_movegen(void)
{
    test_suite("Bishop MoveGen Tests");

    Board board;
    MoveList list;


    /*
     * --------------------------------------------------
     * Fou au centre : 13 déplacements
     * --------------------------------------------------
     *
     * Depuis e4 :
     *
     * d5 c6 b7 a8
     * f5 g6 h7
     * d3 c2 b1
     * f3 g2 h1
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_BISHOP
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(13, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("c6")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("b7")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("a8")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("g6")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("h7")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("c2")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("b1")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("g2")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("h1")
        )
    );


    /*
     * --------------------------------------------------
     * Pièce alliée bloque la diagonale
     * --------------------------------------------------
     *
     * e4 -> f5 est impossible si f5 est occupée
     * par une pièce blanche.
     *
     * g6 et h7 doivent également être inaccessibles.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_BISHOP
    );

    board_set_piece(
        &board,
        coord_from_string("f5"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

   

    ASSERT_INT(
    10,
    count_moves_from(
        &list,
        coord_from_string("e4")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f5")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("g6")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("h7")
        )
    );


    /*
     * --------------------------------------------------
     * Capture d'une pièce ennemie
     * --------------------------------------------------
     *
     * Le fou peut capturer d5, mais ne peut pas
     * continuer vers c6.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_BISHOP
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(10, movelist_count(&list));
    
    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d5")
        )
    );

    ASSERT_TRUE(
        move_is_capture(
            movelist_get(
                &list,
                movelist_find(
                    &list,
                    move_create(
                        coord_from_string("e4"),
                        coord_from_string("d5"),
                        PROMOTION_NONE,
                        MOVE_CAPTURE
                    )
                )
            )
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("c6")
        )
    );


    /*
     * --------------------------------------------------
     * Un fou ne se déplace que sur sa couleur
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_BISHOP
    );

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_PAWN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(13, movelist_count(&list));

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );


    /*
     * --------------------------------------------------
     * Fou noir
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_BISHOP
    );

    board.side_to_move = COLOR_BLACK;

    movegen_generate(&board, &list);

    ASSERT_INT(13, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d6")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("f6")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("d4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("f4")
        )
    );


    /*
     * --------------------------------------------------
     * Un fou noir ne génère pas de coups blancs
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_BISHOP
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(0, movelist_count(&list));
}

void test_rook_movegen(void)
{
    test_suite("Rook MoveGen Tests");

    Board board;
    MoveList list;


    /*
     * --------------------------------------------------
     * Tour au centre
     * --------------------------------------------------
     *
     * Une tour en e4 possède 14 coups :
     *
     * e5 e6 e7 e8
     * e3 e2 e1
     * f4 g4 h4
     * d4 c4 b4 a4
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(14, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e1")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("h4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("a4")
        )
    );


    /*
     * --------------------------------------------------
     * Pièce alliée bloque la tour
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e6"),
        WHITE_PAWN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(
    11,
    count_moves_from(
        &list,
        coord_from_string("e4")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e6")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e7")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );


    /*
     * --------------------------------------------------
     * Capture d'une pièce ennemie
     * --------------------------------------------------
     *
     * La tour peut capturer e6,
     * mais ne peut pas continuer vers e7/e8.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e6"),
        BLACK_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(12, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e6")
        )
    );

    ASSERT_TRUE(
        move_is_capture(
            movelist_get(
                &list,
                movelist_find(
                    &list,
                    move_create(
                        coord_from_string("e4"),
                        coord_from_string("e6"),
                        PROMOTION_NONE,
                        MOVE_CAPTURE
                    )
                )
            )
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e7")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e8")
        )
    );


    /*
     * --------------------------------------------------
     * Tour noire
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_BLACK;

    movegen_generate(&board, &list);

    ASSERT_INT(14, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("e1")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("a5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("h5")
        )
    );


    /*
     * --------------------------------------------------
     * Une tour noire ne génère pas de coups
     * lorsque les blancs sont au trait.
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(0, movelist_count(&list));
}

void test_queen_movegen(void)
{
    test_suite("Queen MoveGen Tests");

    Board board;
    MoveList list;


    /*
     * --------------------------------------------------
     * Dame au centre
     * --------------------------------------------------
     *
     * Une dame en e4 possède 27 coups sur un échiquier
     * vide :
     *
     * 14 coups de tour
     * + 13 coups de fou
     * = 27
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_QUEEN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(27, movelist_count(&list));


    /*
     * Déplacements verticaux.
     */
    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e1")
        )
    );


    /*
     * Déplacements horizontaux.
     */
    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("h4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("a4")
        )
    );


    /*
     * Déplacements diagonaux.
     */
    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("a8")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("h7")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("b1")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("h1")
        )
    );


    /*
     * --------------------------------------------------
     * Pièce alliée bloque la dame
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("e6"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);
    
    ASSERT_INT(
    24,
    count_moves_from(
        &list,
        coord_from_string("e4")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e6")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e7")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e8")
        )
    );

    /*
     * La case juste avant la pièce reste accessible.
     */
    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );


    /*
     * --------------------------------------------------
     * Capture d'une pièce ennemie
     * --------------------------------------------------
     *
     * La dame peut capturer d5, mais ne peut pas
     * continuer vers c6.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("d5"),
        BLACK_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(24, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d5")
        )
    );

    ASSERT_TRUE(
        move_is_capture(
            movelist_get(
                &list,
                movelist_find(
                    &list,
                    move_create(
                        coord_from_string("e4"),
                        coord_from_string("d5"),
                        PROMOTION_NONE,
                        MOVE_CAPTURE
                    )
                )
            )
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("c6")
        )
    );


    /*
     * --------------------------------------------------
     * Dame noire
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_QUEEN
    );

    board.side_to_move = COLOR_BLACK;

    movegen_generate(&board, &list);

    ASSERT_INT(27, movelist_count(&list));

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("a5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("a1")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e5"),
            coord_from_string("h8")
        )
    );


    /*
     * --------------------------------------------------
     * Une dame noire ne génère pas de coups
     * lorsque les blancs sont au trait.
     * --------------------------------------------------
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_QUEEN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(0, movelist_count(&list));
}

void test_king_movegen(void)
{
    test_suite("King MoveGen Tests");

    Board board;
    MoveList list;


    /*
     * Roi au centre : 8 coups.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(
        8,
        count_moves_from(
            &list,
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f5")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("d3")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("f3")
        )
    );


    /*
     * Roi dans un coin : 3 coups.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(
        3,
        count_moves_from(
            &list,
            coord_from_string("a1")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("a1"),
            coord_from_string("a2")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("a1"),
            coord_from_string("b1")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("a1"),
            coord_from_string("b2")
        )
    );


    /*
     * Pièce alliée bloque le roi.
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
        WHITE_PAWN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(
        7,
        count_moves_from(
            &list,
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        0,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );


    /*
     * Capture d'une pièce adverse.
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
        BLACK_QUEEN
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(
        8,
        count_moves_from(
            &list,
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        1,
        has_move(
            &list,
            coord_from_string("e4"),
            coord_from_string("e5")
        )
    );

    const Move *capture = NULL;

    for (int i = 0; i < movelist_count(&list); i++)
    {
        const Move *move = movelist_get(&list, i);

        if (move->from == coord_from_string("e4") &&
            move->to == coord_from_string("e5"))
        {
            capture = move;
            break;
        }
    }

    ASSERT_TRUE(capture != NULL);

    if (capture != NULL)
    {
        ASSERT_TRUE(move_is_capture(capture));
    }


    /*
     * Roi noir.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_KING
    );

    board.side_to_move = COLOR_BLACK;

    movegen_generate(&board, &list);

    ASSERT_INT(
        8,
        count_moves_from(
            &list,
            coord_from_string("e5")
        )
    );


    /*
     * Le roi noir ne doit pas jouer
     * lorsque les blancs sont au trait.
     */
    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e5"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    movegen_generate(&board, &list);

    ASSERT_INT(
        0,
        count_moves_from(
            &list,
            coord_from_string("e5")
        )
    );
}