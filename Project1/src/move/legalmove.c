#include "move/legalmove.h"

#include "attack/attack.h"
#include "move/makemove.h"
#include "move/movegen.h"


void legalmove_generate(Board *board,
                        MoveList *list)
{
    CHESS_ASSERT(board != NULL);
    CHESS_ASSERT(list != NULL);

    MoveList pseudo_moves;

    movegen_generate(
        board,
        &pseudo_moves
    );

    movelist_clear(list);

    PieceColor moving_side =
        board->side_to_move;


    for (int i = 0;
         i < movelist_count(&pseudo_moves);
         i++)
    {

        Move move =
            *movelist_get(
                &pseudo_moves,
                i
            );

        Undo undo;

        make_move(
            board,
            &move,
            &undo
        );

        if (!board_is_in_check(
                board,
                moving_side
            ))
        {
            movelist_add(
                list,
                move
            );
        }

        undo_move(
            board,
            &move,
            &undo
        );
    }
}
