#include "board/board.h"
#include "coord/coord.h"
#include "move/legalmove.h"
#include "move/makemove.h"
#include "move/move.h"
#include "move/movelist.h"
#include "piece/piece.h"
#include "search/search.h"
#include "test.h"


static int same_move(const Move *move,
                     int from,
                     int to)
{
    return move->from == from &&
           move->to == to;
}


void test_search(void)
{
    test_suite("Search Tests");

    Board board;
    Move best_move;


    /*
     * ========================================
     * L'IA BLANCHE CAPTURE UNE DAME
     * ========================================
     *
     * Blanc :
     * Roi  e1
     * Tour a1
     *
     * Noir :
     * Roi  e8
     * Dame a8
     *
     * La tour blanche peut capturer la dame :
     *
     * a1 -> a8
     *
     * A profondeur 1, c'est immédiatement
     * le meilleur gain matériel.
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
        coord_from_string("e8"),
        BLACK_KING
    );

    board_set_piece(
        &board,
        coord_from_string("a8"),
        BLACK_QUEEN
    );

    board.side_to_move = COLOR_WHITE;

    ASSERT_TRUE(
        search_best_move(
            &board,
            1,
            &best_move
        )
    );

    ASSERT_TRUE(
        same_move(
            &best_move,
            coord_from_string("a1"),
            coord_from_string("a8")
        )
    );

    ASSERT_TRUE(
        move_is_capture(
            &best_move
        )
    );


    /*
     * ========================================
     * L'IA NOIRE CAPTURE UNE DAME
     * ========================================
     *
     * Test miroir.
     *
     * Noir doit chercher le score minimum,
     * donc capturer la dame blanche.
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
        WHITE_QUEEN
    );

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

    ASSERT_TRUE(
        search_best_move(
            &board,
            1,
            &best_move
        )
    );

    ASSERT_TRUE(
        same_move(
            &best_move,
            coord_from_string("a8"),
            coord_from_string("a1")
        )
    );

    ASSERT_TRUE(
        move_is_capture(
            &best_move
        )
    );


    /*
     * ========================================
     * MAT EN 1 POUR LES BLANCS
     * ========================================
     *
     * Position :
     *
     * Roi noir   : h8
     * Roi blanc  : f6
     * Dame blanche : g6
     *
     * Blanc peut jouer :
     *
     * g6 -> g7#
     *
     * Après ce coup :
     *
     * - le roi noir est attaqué ;
     * - h7 est contrôlée ;
     * - g8 est contrôlée ;
     * - aucune échappatoire.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("f6"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("g6"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    board.side_to_move = COLOR_WHITE;

    ASSERT_TRUE(
        search_best_move(
            &board,
            1,
            &best_move
        )
    );


    /*
     * On ne se contente pas ici de vérifier
     * le from/to.
     *
     * On joue le coup choisi puis on vérifie
     * que la position résultante est bien mat.
     */

    Undo undo;

    make_move(
        &board,
        &best_move,
        &undo
    );

    ASSERT_TRUE(
        board_is_checkmate(
            &board
        )
    );

    undo_move(
        &board,
        &best_move,
        &undo
    );


    /*
     * ========================================
     * MAT EN 1 PRIORITAIRE SUR LE MATERIEL
     * ========================================
     *
     * Le moteur ne doit pas préférer simplement
     * une capture matérielle si un mat immédiat
     * est disponible.
     */

    board_clear(&board);

    board_set_piece(
        &board,
        coord_from_string("f6"),
        WHITE_KING
    );

    board_set_piece(
        &board,
        coord_from_string("g6"),
        WHITE_QUEEN
    );

    board_set_piece(
        &board,
        coord_from_string("h8"),
        BLACK_KING
    );

    /*
     * Pièce noire capturable pour tenter
     * de distraire l'IA.
     */
    board_set_piece(
        &board,
        coord_from_string("c2"),
        BLACK_ROOK
    );

    board.side_to_move = COLOR_WHITE;

    ASSERT_TRUE(
        search_best_move(
            &board,
            1,
            &best_move
        )
    );

    make_move(
        &board,
        &best_move,
        &undo
    );

    ASSERT_TRUE(
        board_is_checkmate(
            &board
        )
    );

    undo_move(
        &board,
        &best_move,
        &undo
    );


    /*
     * ========================================
     * AUCUN COUP : ECHEC ET MAT
     * ========================================
     *
     * Noir est déjà mat.
     *
     * search_best_move() doit retourner 0.
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
        board_is_checkmate(
            &board
        )
    );

    ASSERT_FALSE(
        search_best_move(
            &board,
            3,
            &best_move
        )
    );


    /*
     * ========================================
     * AUCUN COUP : PAT
     * ========================================
     *
     * Roi noir : h8
     * Roi blanc : f7
     * Dame blanche : g6
     *
     * Noir n'est pas en échec mais ne possède
     * aucun coup légal.
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
        board_is_stalemate(
            &board
        )
    );

    ASSERT_FALSE(
        search_best_move(
            &board,
            3,
            &best_move
        )
    );


    /*
     * ========================================
     * POSITION INITIALE
     * ========================================
     *
     * L'objectif ici n'est pas de savoir quel
     * coup précis sera choisi.
     *
     * Plusieurs coups ont exactement le même
     * score avec notre évaluation matérielle.
     *
     * On vérifie seulement :
     *
     * - qu'un coup est trouvé ;
     * - qu'il appartient aux coups légaux.
     */

    board_init(&board);

    ASSERT_TRUE(
        search_best_move(
            &board,
            1,
            &best_move
        )
    );

    MoveList legal_moves;

    legalmove_generate(
        &board,
        &legal_moves
    );

    int found = 0;

    for (int i = 0;
         i < movelist_count(&legal_moves);
         i++)
    {
        const Move *move =
            movelist_get(
                &legal_moves,
                i
            );

        if (move->from == best_move.from &&
            move->to == best_move.to &&
            move->promotion == best_move.promotion &&
            move->flags == best_move.flags)
        {
            found = 1;
            break;
        }
    }

    ASSERT_TRUE(found);


    /*
     * ========================================
     * PROFONDEUR 0
     * ========================================
     *
     * Notre API convertit depth < 1 en
     * profondeur 1.
     *
     * Elle doit donc quand même retourner
     * un coup.
     */

    board_init(&board);

    ASSERT_TRUE(
        search_best_move(
            &board,
            0,
            &best_move
        )
    );
}
