#include "attack/attack.h"
#include "board/board.h"
#include "coord/coord.h"
#include "move/legalmove.h"
#include "move/move.h"
#include "move/movelist.h"
#include "piece/piece.h"
#include "test.h"


static int has_move(const MoveList *list,
                    int from,
                    int to)
{
    for (int i = 0; i < movelist_count(list); i++)
    {
        const Move *move = movelist_get(list, i);

        if (move->from == from &&
            move->to == to)
        {
            return 1;
        }
    }

    return 0;
}


static int count_moves_from(const MoveList *list,
                            int from)
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


void test_legalmove(void)
{
    test_suite("LegalMove Tests");

    Board board;
    MoveList list;


    /*
     * ========================================
     * POSITION NORMALE
     * ========================================
     *
     * Dans la position initiale, les 20 coups
     * pseudo-légaux sont également légaux.
     */

    board_init(&board);

    legalmove_generate(&board, &list);

    ASSERT_INT(
        20,
        movelist_count(&list)
    );

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e4")
        )
    );

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("g1"),
            coord_from_string("f3")
        )
    );


    /*
     * ========================================
     * PIECE CLOUEE
     * ========================================
     *
     * Position :
     *
     *      e8 : tour noire
     *      e2 : tour blanche
     *      e1 : roi blanc
     *
     * La tour blanche protège le roi.
     *
     * Elle peut monter/descendre sur la colonne e,
     * mais elle ne peut pas quitter cette colonne,
     * sinon la tour noire donnerait échec au roi.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("e2"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_ROOK
    );

    /*
     * On ajoute le roi noir pour garder
     * une position cohérente.
     */
    board_set_piece(
        &board,
        coord_from_string("a8"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    legalmove_generate(&board, &list);


    /*
     * La tour ne peut pas quitter la colonne e.
     */

    ASSERT_FALSE(
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("d2")
        )
    );

    ASSERT_FALSE(
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("f2")
        )
    );


    /*
     * En revanche, elle peut rester sur la colonne e.
     */

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e3")
        )
    );

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e7")
        )
    );

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("e2"),
            coord_from_string("e8")
        )
    );


    /*
     * ========================================
     * ROI VERS CASE ATTAQUEE
     * ========================================
     *
     * Roi blanc e1.
     * Tour noire d8.
     *
     * La colonne d est attaquée.
     *
     * Le roi ne doit donc pas pouvoir jouer e1 -> d1.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("d8"),
        BLACK_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    legalmove_generate(&board, &list);

    ASSERT_FALSE(
        has_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("d1")
        )
    );


    /*
     * Mais e1 -> f1 n'est pas attaqué par cette tour.
     */

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("f1")
        )
    );


    /*
     * ========================================
     * ROI ADJACENT AU ROI ADVERSE
     * ========================================
     *
     * Un roi ne peut pas aller à côté de l'autre roi.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("e3"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    legalmove_generate(&board, &list);

    /*
     * e2 est attaquée par le roi noir situé en e3.
     */
    ASSERT_FALSE(
        has_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("e2")
        )
    );


    /*
     * ========================================
     * JOUEUR EN ECHEC
     * ========================================
     *
     * Tour noire en e8 donnant échec au roi blanc e1.
     *
     * Une pièce blanche ailleurs ne doit pas pouvoir
     * jouer un coup qui ignore l'échec.
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
        coord_from_string("a2"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );

    legalmove_generate(&board, &list);


    /*
     * a2 -> a3 ne répond absolument pas à l'échec.
     * Il doit donc être supprimé.
     */

    ASSERT_FALSE(
        has_move(
            &list,
            coord_from_string("a2"),
            coord_from_string("a3")
        )
    );


    /*
     * Le roi doit malgré tout disposer de certaines
     * cases de fuite.
     */

    ASSERT_TRUE(
        count_moves_from(
            &list,
            coord_from_string("e1")
        ) > 0
    );


    /*
     * ========================================
     * BLOQUER UN ECHEC
     * ========================================
     *
     * Tour noire e8 -> roi blanc e1.
     * Tour blanche a2.
     *
     * La tour blanche peut jouer a2 -> e2,
     * ce qui bloque la ligne d'attaque.
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
        coord_from_string("a2"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    legalmove_generate(&board, &list);

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("a2"),
            coord_from_string("e2")
        )
    );


    /*
     * ========================================
     * CAPTURER LA PIECE QUI DONNE ECHEC
     * ========================================
     *
     * Une tour blanche peut capturer la tour noire
     * qui donne échec si le chemin est libre.
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
        coord_from_string("a8"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("h7"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    legalmove_generate(&board, &list);

    ASSERT_TRUE(
        has_move(
            &list,
            coord_from_string("a8"),
            coord_from_string("e8")
        )
    );


    /*
     * ========================================
     * LE BOARD DOIT RESTER INCHANGE
     * ========================================
     *
     * legalmove_generate() utilise make/undo
     * en interne. Il ne doit donc pas modifier
     * la position originale.
     */

    board_init(&board);

    Board before = board;

    legalmove_generate(&board, &list);

    for (int square = 0; square < 128; square++)
    {
        ASSERT_INT(
            before.squares[square],
            board.squares[square]
        );
    }

    ASSERT_INT(
        before.side_to_move,
        board.side_to_move
    );

    ASSERT_INT(
        before.castling_rights,
        board.castling_rights
    );

    ASSERT_INT(
        before.en_passant_square,
        board.en_passant_square
    );

    ASSERT_INT(
        before.halfmove_clock,
        board.halfmove_clock
    );

    ASSERT_INT(
        before.fullmove_number,
        board.fullmove_number
    );
}
