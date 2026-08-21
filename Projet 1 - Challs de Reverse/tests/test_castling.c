#include "attack/attack.h"
#include "board/board.h"
#include "coord/coord.h"
#include "move/legalmove.h"
#include "move/makemove.h"
#include "move/move.h"
#include "move/movegen.h"
#include "move/movelist.h"
#include "piece/piece.h"
#include "test.h"


static const Move *find_castle_move(const MoveList *list,
                                    int from,
                                    int to)
{
    for (int i = 0; i < movelist_count(list); i++)
    {
        const Move *move = movelist_get(list, i);

        if (move->from == from &&
            move->to == to &&
            move_is_castle(move))
        {
            return move;
        }
    }

    return NULL;
}


void test_castling(void)
{
    test_suite("Castling Tests");

    Board board;
    MoveList list;
    Undo undo;


    /*
     * ========================================
     * PETIT ROQUE BLANC
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
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE;

    movegen_generate(
        &board,
        &list
    );

    const Move *white_kingside =
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        );

    ASSERT_TRUE(
        white_kingside != NULL
    );


    /*
     * ========================================
     * GRAND ROQUE BLANC
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
        coord_from_string("a1"),
        WHITE_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_QUEENSIDE;

    movegen_generate(
        &board,
        &list
    );

    const Move *white_queenside =
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("c1")
        );

    ASSERT_TRUE(
        white_queenside != NULL
    );


    /*
     * ========================================
     * PETIT ROQUE NOIR
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
        coord_from_string("h8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_BLACK;

    board.castling_rights =
        CASTLE_BLACK_KINGSIDE;

    movegen_generate(
        &board,
        &list
    );

    const Move *black_kingside =
        find_castle_move(
            &list,
            coord_from_string("e8"),
            coord_from_string("g8")
        );

    ASSERT_TRUE(
        black_kingside != NULL
    );


    /*
     * ========================================
     * GRAND ROQUE NOIR
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
        coord_from_string("a8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_BLACK;

    board.castling_rights =
        CASTLE_BLACK_QUEENSIDE;

    movegen_generate(
        &board,
        &list
    );

    const Move *black_queenside =
        find_castle_move(
            &list,
            coord_from_string("e8"),
            coord_from_string("c8")
        );

    ASSERT_TRUE(
        black_queenside != NULL
    );


    /*
     * ========================================
     * CASE OCCUPEE : PETIT ROQUE BLANC
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
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("f1"),
        WHITE_BISHOP
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        ) == NULL
    );


    /*
     * ========================================
     * CASE OCCUPEE : GRAND ROQUE BLANC
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
        coord_from_string("a1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("d1"),
        WHITE_QUEEN
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_QUEENSIDE;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("c1")
        ) == NULL
    );


    /*
     * ========================================
     * ROI DEJA EN ECHEC
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
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE;

    ASSERT_TRUE(
        board_is_in_check(
            &board,
            COLOR_WHITE
        )
    );

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        ) == NULL
    );


    /*
     * ========================================
     * CASE TRAVERSEE ATTAQUEE
     * ========================================
     *
     * f1 est attaquée par une tour noire
     * située en f8.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("f8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        ) == NULL
    );


    /*
     * ========================================
     * CASE D'ARRIVEE ATTAQUEE
     * ========================================
     *
     * g1 est attaquée par la tour noire g8.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("g8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        ) == NULL
    );


    /*
     * ========================================
     * ABSENCE DE TOUR
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        ) == NULL
    );


    /*
     * ========================================
     * AUCUN DROIT DE ROQUE
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
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board.side_to_move = COLOR_WHITE;
    board.castling_rights = 0;

    movegen_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        ) == NULL
    );


    /*
     * ========================================
     * MAKE MOVE : PETIT ROQUE BLANC
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
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE |
        CASTLE_WHITE_QUEENSIDE;

    Move castle =
        move_create(
            coord_from_string("e1"),
            coord_from_string("g1"),
            PROMOTION_NONE,
            MOVE_CASTLE
        );

    make_move(
        &board,
        &castle,
        &undo
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e1")
        )
    );

    ASSERT_INT(
        WHITE_KING,
        board_get_piece(
            &board,
            coord_from_string("g1")
        )
    );

    ASSERT_INT(
        WHITE_ROOK,
        board_get_piece(
            &board,
            coord_from_string("f1")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("h1")
        )
    );


    /*
     * Après déplacement du roi, les deux droits
     * blancs doivent disparaître.
     */

    ASSERT_FALSE(
        board.castling_rights &
        CASTLE_WHITE_KINGSIDE
    );

    ASSERT_FALSE(
        board.castling_rights &
        CASTLE_WHITE_QUEENSIDE
    );


    /*
     * ========================================
     * UNDO PETIT ROQUE BLANC
     * ========================================
     */

    undo_move(
        &board,
        &castle,
        &undo
    );

    ASSERT_INT(
        WHITE_KING,
        board_get_piece(
            &board,
            coord_from_string("e1")
        )
    );

    ASSERT_INT(
        WHITE_ROOK,
        board_get_piece(
            &board,
            coord_from_string("h1")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("f1")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("g1")
        )
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_WHITE_KINGSIDE
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_WHITE_QUEENSIDE
    );


    /*
     * ========================================
     * MAKE + UNDO GRAND ROQUE NOIR
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
        coord_from_string("a8"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_BLACK;

    board.castling_rights =
        CASTLE_BLACK_KINGSIDE |
        CASTLE_BLACK_QUEENSIDE;

    Move black_castle =
        move_create(
            coord_from_string("e8"),
            coord_from_string("c8"),
            PROMOTION_NONE,
            MOVE_CASTLE
        );

    make_move(
        &board,
        &black_castle,
        &undo
    );

    ASSERT_INT(
        BLACK_KING,
        board_get_piece(
            &board,
            coord_from_string("c8")
        )
    );

    ASSERT_INT(
        BLACK_ROOK,
        board_get_piece(
            &board,
            coord_from_string("d8")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("a8")
        )
    );

    undo_move(
        &board,
        &black_castle,
        &undo
    );

    ASSERT_INT(
        BLACK_KING,
        board_get_piece(
            &board,
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        BLACK_ROOK,
        board_get_piece(
            &board,
            coord_from_string("a8")
        )
    );


    /*
     * ========================================
     * ROI BLANC BOUGE : PERTE DES 2 DROITS
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e1"),
        WHITE_KING
    );

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE |
        CASTLE_WHITE_QUEENSIDE |
        CASTLE_BLACK_KINGSIDE |
        CASTLE_BLACK_QUEENSIDE;

    board.side_to_move = COLOR_WHITE;

    Move king_move =
        move_create(
            coord_from_string("e1"),
            coord_from_string("e2"),
            PROMOTION_NONE,
            MOVE_NONE
        );

    make_move(
        &board,
        &king_move,
        &undo
    );

    ASSERT_FALSE(
        board.castling_rights &
        CASTLE_WHITE_KINGSIDE
    );

    ASSERT_FALSE(
        board.castling_rights &
        CASTLE_WHITE_QUEENSIDE
    );

    /*
     * Les droits noirs doivent rester.
     */
    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_BLACK_KINGSIDE
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_BLACK_QUEENSIDE
    );


    /*
     * ========================================
     * TOUR h1 BOUGE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE |
        CASTLE_WHITE_QUEENSIDE;

    board.side_to_move = COLOR_WHITE;

    Move rook_h1 =
        move_create(
            coord_from_string("h1"),
            coord_from_string("h2"),
            PROMOTION_NONE,
            MOVE_NONE
        );

    make_move(
        &board,
        &rook_h1,
        &undo
    );

    ASSERT_FALSE(
        board.castling_rights &
        CASTLE_WHITE_KINGSIDE
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_WHITE_QUEENSIDE
    );


    /*
     * ========================================
     * TOUR a1 BOUGE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("a1"),
        WHITE_ROOK
    );

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE |
        CASTLE_WHITE_QUEENSIDE;

    board.side_to_move = COLOR_WHITE;

    Move rook_a1 =
        move_create(
            coord_from_string("a1"),
            coord_from_string("a2"),
            PROMOTION_NONE,
            MOVE_NONE
        );

    make_move(
        &board,
        &rook_a1,
        &undo
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_WHITE_KINGSIDE
    );

    ASSERT_FALSE(
        board.castling_rights &
        CASTLE_WHITE_QUEENSIDE
    );


    /*
     * ========================================
     * CAPTURE D'UNE TOUR SUR h8
     * ========================================
     *
     * Si la tour noire de h8 est capturée,
     * le petit roque noir disparaît.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_ROOK
    );

    board.castling_rights =
        CASTLE_BLACK_KINGSIDE |
        CASTLE_BLACK_QUEENSIDE;

    board.side_to_move = COLOR_WHITE;

    Move capture_rook =
        move_create(
            coord_from_string("h1"),
            coord_from_string("h8"),
            PROMOTION_NONE,
            MOVE_CAPTURE
        );

    make_move(
        &board,
        &capture_rook,
        &undo
    );

    ASSERT_FALSE(
        board.castling_rights &
        CASTLE_BLACK_KINGSIDE
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_BLACK_QUEENSIDE
    );


    /*
     * ========================================
     * UNDO RESTAURE LES DROITS
     * ========================================
     */

    undo_move(
        &board,
        &capture_rook,
        &undo
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_BLACK_KINGSIDE
    );

    ASSERT_TRUE(
        board.castling_rights &
        CASTLE_BLACK_QUEENSIDE
    );


    /*
     * ========================================
     * LEGALMOVE + ROQUE
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
        coord_from_string("h1"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights =
        CASTLE_WHITE_KINGSIDE;

    legalmove_generate(
        &board,
        &list
    );

    ASSERT_TRUE(
        find_castle_move(
            &list,
            coord_from_string("e1"),
            coord_from_string("g1")
        ) != NULL
    );
}
