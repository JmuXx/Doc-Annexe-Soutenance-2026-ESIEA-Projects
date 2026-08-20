#include "board/board.h"
#include "coord/coord.h"
#include "move/makemove.h"
#include "move/move.h"
#include "piece/piece.h"
#include "test.h"


/*
 * Compare entièrement deux Board.
 *
 * Retourne 1 s'ils sont identiques,
 * 0 sinon.
 */
static int boards_equal(const Board *a,
                        const Board *b)
{
    for (int square = 0; square < 128; square++)
    {
        if (a->squares[square] != b->squares[square])
        {
            return 0;
        }
    }

    if (a->side_to_move != b->side_to_move)
    {
        return 0;
    }

    if (a->castling_rights != b->castling_rights)
    {
        return 0;
    }

    if (a->en_passant_square != b->en_passant_square)
    {
        return 0;
    }

    if (a->halfmove_clock != b->halfmove_clock)
    {
        return 0;
    }

    if (a->fullmove_number != b->fullmove_number)
    {
        return 0;
    }

    return 1;
}


void test_makemove(void)
{
    test_suite("MakeMove Tests");

    Board board;
    Board before;

    Move move;
    Undo undo;


    /*
     * ========================================
     * COUP SIMPLE DE PION
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e2"),
        WHITE_PAWN
    );

    board.side_to_move = COLOR_WHITE;

    move = move_create(
        coord_from_string("e2"),
        coord_from_string("e4"),
        PROMOTION_NONE,
        MOVE_NONE
    );

    make_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e2")
        )
    );

    ASSERT_INT(
        WHITE_PAWN,
        board_get_piece(
            &board,
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        COLOR_BLACK,
        board.side_to_move
    );


    /*
     * Un coup de pion remet le halfmove_clock à 0.
     */
    ASSERT_INT(
        0,
        board.halfmove_clock
    );


    /*
     * Le coup blanc ne change pas encore
     * fullmove_number.
     */
    ASSERT_INT(
        1,
        board.fullmove_number
    );


    /*
     * ========================================
     * UNDO DU COUP SIMPLE
     * ========================================
     */

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        WHITE_PAWN,
        board_get_piece(
            &board,
            coord_from_string("e2")
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
        COLOR_WHITE,
        board.side_to_move
    );


    /*
     * ========================================
     * CAPTURE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("e4"),
        WHITE_ROOK
    );

    board_set_piece(
        &board,
        coord_from_string("e8"),
        BLACK_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    move = move_create(
        coord_from_string("e4"),
        coord_from_string("e8"),
        PROMOTION_NONE,
        MOVE_CAPTURE
    );

    make_move(
        &board,
        &move,
        &undo
    );


    /*
     * La tour doit maintenant être en e8.
     */
    ASSERT_INT(
        WHITE_ROOK,
        board_get_piece(
            &board,
            coord_from_string("e8")
        )
    );

    ASSERT_INT(
        EMPTY,
        board_get_piece(
            &board,
            coord_from_string("e4")
        )
    );


    /*
     * La pièce capturée doit avoir été sauvegardée.
     */
    ASSERT_INT(
        BLACK_KNIGHT,
        undo.captured_piece
    );


    /*
     * Une capture remet le halfmove_clock à 0.
     */
    ASSERT_INT(
        0,
        board.halfmove_clock
    );


    /*
     * ========================================
     * UNDO D'UNE CAPTURE
     * ========================================
     */

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        WHITE_ROOK,
        board_get_piece(
            &board,
            coord_from_string("e4")
        )
    );

    ASSERT_INT(
        BLACK_KNIGHT,
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
     * HALFMOVE CLOCK
     * ========================================
     *
     * Un déplacement d'une pièce autre qu'un pion,
     * sans capture, incrémente le compteur.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("g1"),
        WHITE_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    board.halfmove_clock = 12;

    move = move_create(
        coord_from_string("g1"),
        coord_from_string("f3"),
        PROMOTION_NONE,
        MOVE_NONE
    );

    make_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        13,
        board.halfmove_clock
    );

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        12,
        board.halfmove_clock
    );


    /*
     * ========================================
     * FULLMOVE NUMBER
     * ========================================
     *
     * Le compteur augmente après un coup noir.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("g8"),
        BLACK_KNIGHT
    );

    board.side_to_move = COLOR_BLACK;
    board.fullmove_number = 7;

    move = move_create(
        coord_from_string("g8"),
        coord_from_string("f6"),
        PROMOTION_NONE,
        MOVE_NONE
    );

    make_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        8,
        board.fullmove_number
    );

    ASSERT_INT(
        COLOR_WHITE,
        board.side_to_move
    );

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        7,
        board.fullmove_number
    );

    ASSERT_INT(
        COLOR_BLACK,
        board.side_to_move
    );


    /*
     * ========================================
     * EN PASSANT SQUARE
     * ========================================
     *
     * make_move() efface actuellement cette valeur,
     * mais undo_move() doit restaurer l'ancienne.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("g1"),
        WHITE_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;

    board.en_passant_square =
        coord_from_string("e6");

    move = move_create(
        coord_from_string("g1"),
        coord_from_string("f3"),
        PROMOTION_NONE,
        MOVE_NONE
    );

    make_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        -1,
        board.en_passant_square
    );

    ASSERT_INT(
        coord_from_string("e6"),
        undo.en_passant_square
    );

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        coord_from_string("e6"),
        board.en_passant_square
    );


    /*
     * ========================================
     * CASTLING RIGHTS
     * ========================================
     *
     * make_move() ne les modifie pas encore,
     * mais Undo doit déjà les sauvegarder.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("g1"),
        WHITE_KNIGHT
    );

    board.side_to_move = COLOR_WHITE;
    board.castling_rights = 0xB;

    move = move_create(
        coord_from_string("g1"),
        coord_from_string("f3"),
        PROMOTION_NONE,
        MOVE_NONE
    );

    make_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        0xB,
        undo.castling_rights
    );

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_INT(
        0xB,
        board.castling_rights
    );


    /*
     * ========================================
     * RESTAURATION COMPLETE DU BOARD
     * ========================================
     *
     * C'est le test le plus important.
     *
     * Après :
     *
     * make_move()
     * undo_move()
     *
     * le Board doit être strictement identique.
     */

    board_init(&board);

    board.halfmove_clock = 17;

    board.en_passant_square =
        coord_from_string("d6");

    board.castling_rights = 0xD;

    before = board;


    move = move_create(
        coord_from_string("g1"),
        coord_from_string("f3"),
        PROMOTION_NONE,
        MOVE_NONE
    );

    make_move(
        &board,
        &move,
        &undo
    );

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_TRUE(
        boards_equal(
            &board,
            &before
        )
    );


    /*
     * ========================================
     * RESTAURATION COMPLETE APRES CAPTURE
     * ========================================
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("c4"),
        WHITE_BISHOP
    );

    board_set_piece(
        &board,
        coord_from_string("f7"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    board.castling_rights = 7;
    board.en_passant_square = 42;
    board.halfmove_clock = 23;
    board.fullmove_number = 11;

    before = board;

    move = move_create(
        coord_from_string("c4"),
        coord_from_string("f7"),
        PROMOTION_NONE,
        MOVE_CAPTURE
    );

    make_move(
        &board,
        &move,
        &undo
    );

    undo_move(
        &board,
        &move,
        &undo
    );

    ASSERT_TRUE(
        boards_equal(
            &board,
            &before
        )
    );
}
