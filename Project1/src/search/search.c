#include <stdio.h>

#include "search/search.h"
#include "board/board.h"
#include "move/legalmove.h"
#include "move/makemove.h"
#include "move/movelist.h"
#include "piece/piece.h"
#include "attack/attack.h"
#include "search/evaluate.h"


#define SEARCH_INF   100000000
#define SEARCH_MATE  1000000


typedef struct
{
    Move move;
    int score;

} ScoredMove;


static int move_order_piece_value(Piece piece)
{
    if (piece_is_pawn(piece))
    {
        return 100;
    }

    if (piece_is_knight(piece))
    {
        return 320;
    }

    if (piece_is_bishop(piece))
    {
        return 330;
    }

    if (piece_is_rook(piece))
    {
        return 500;
    }

    if (piece_is_queen(piece))
    {
        return 900;
    }

    if (piece_is_king(piece))
    {
        return 20000;
    }

    return 0;
}

static int score_move(Board *board,
                      const Move *move)
{
    int score = 0;

    Piece moving_piece =
        board_get_piece(
            board,
            move->from
        );

    if (move_is_promotion(move))
    {
        score += 10000;

        switch (move->promotion)
        {
            case PROMOTION_QUEEN:

                score += 900;
                break;


            case PROMOTION_ROOK:

                score += 500;
                break;


            case PROMOTION_BISHOP:

                score += 330;
                break;


            case PROMOTION_KNIGHT:

                score += 320;
                break;


            default:

                break;
        }
    }


    if (move_is_capture(move))
    {
        Piece captured_piece =
            EMPTY;

        if (move_is_enpassant(move))
        {
            int captured_square;


            if (piece_is_white(moving_piece))
            {
                captured_square =
                    move->to - 16;
            }
            else
            {
                captured_square =
                    move->to + 16;
            }


            captured_piece =
                board_get_piece(
                    board,
                    captured_square
                );
        }
        else
        {
            captured_piece =
                board_get_piece(
                    board,
                    move->to
                );
        }

        score += 5000;
        score +=
            10 *
            move_order_piece_value(
                captured_piece
            );

        score -=
            move_order_piece_value(
                moving_piece
            );
    }

    if (move_is_castle(move))
    {
        score += 1000;
    }

    Undo undo;

    make_move(
        board,
        move,
        &undo
    );

    if (board_is_in_check(
            board,
            board->side_to_move
        ))
    {
        score += 2000;
    }

    undo_move(
        board,
        move,
        &undo
    );


    return score;
}


static void order_moves(Board *board,
                        const MoveList *list,
                        ScoredMove ordered[],
                        int count)
{

    for (int i = 0;
         i < count;
         i++)
    {
        ordered[i].move =
            *movelist_get(
                list,
                i
            );


        ordered[i].score =
            score_move(
                board,
                &ordered[i].move
            );
    }

    for (int i = 0;
         i < count - 1;
         i++)
    {
        int best_index =
            i;


        for (int j = i + 1;
             j < count;
             j++)
        {
            if (ordered[j].score >
                ordered[best_index].score)
            {
                best_index =
                    j;
            }
        }


        if (best_index != i)
        {
            ScoredMove tmp =
                ordered[i];

            ordered[i] =
                ordered[best_index];

            ordered[best_index] =
                tmp;
        }
    }
}


static int minimax(Board *board,
                   int depth,
                   int alpha,
                   int beta)
{

    if (board_is_checkmate(board))
    {
        if (board->side_to_move == COLOR_WHITE)
        {
            return -SEARCH_MATE;
        }

        return SEARCH_MATE;
    }

    if (board_is_stalemate(board))
    {
        return 0;
    }

    if (depth <= 0)
    {
        return evaluate_board(board);
    }

    MoveList legal_moves;


    legalmove_generate(
        board,
        &legal_moves
    );

    int move_count =
        movelist_count(
            &legal_moves
        );

    ScoredMove ordered_moves[256];

    order_moves(
        board,
        &legal_moves,
        ordered_moves,
        move_count
    );

    if (board->side_to_move == COLOR_WHITE)
    {
        int best_score =
            -SEARCH_INF;

        for (int i = 0;
             i < move_count;
             i++)
        {
            const Move *move =
                &ordered_moves[i].move;


            Undo undo;


            make_move(
                board,
                move,
                &undo
            );


            int score =
                minimax(
                    board,
                    depth - 1,
                    alpha,
                    beta
                );


            undo_move(
                board,
                move,
                &undo
            );


            if (score > best_score)
            {
                best_score =
                    score;
            }

            if (best_score > alpha)
            {
                alpha =
                    best_score;
            }

            if (beta <= alpha)
            {
                break;
            }
        }


        return best_score;
    }

    int best_score =
        SEARCH_INF;


    for (int i = 0;
         i < move_count;
         i++)
    {
        const Move *move =
            &ordered_moves[i].move;


        Undo undo;


        make_move(
            board,
            move,
            &undo
        );


        int score =
            minimax(
                board,
                depth - 1,
                alpha,
                beta
            );


        undo_move(
            board,
            move,
            &undo
        );


        if (score < best_score)
        {
            best_score =
                score;
        }

        if (best_score < beta)
        {
            beta =
                best_score;
        }

        if (beta <= alpha)
        {
            break;
        }
    }


    return best_score;
}


int search_best_move(Board *board,
                     int depth,
                     Move *best_move)
{
    CHESS_ASSERT(board != NULL);
    CHESS_ASSERT(best_move != NULL);

    if (depth < 1)
    {
        depth = 1;
    }

    MoveList legal_moves;

    legalmove_generate(
        board,
        &legal_moves
    );

    int move_count =
        movelist_count(
            &legal_moves
        );

    if (move_count == 0)
    {
        return 0;
    }

    ScoredMove ordered_moves[256];

    order_moves(
        board,
        &legal_moves,
        ordered_moves,
        move_count
    );

    *best_move =
        ordered_moves[0].move;

    int alpha =
        -SEARCH_INF;

    int beta =
        SEARCH_INF;

    if (board->side_to_move == COLOR_WHITE)
    {
        int best_score =
            -SEARCH_INF;

        for (int i = 0;
             i < move_count;
             i++)
        {
            const Move *move =
                &ordered_moves[i].move;

            Undo undo;

            make_move(
                board,
                move,
                &undo
            );

            int score =
                minimax(
                    board,
                    depth - 1,
                    alpha,
                    beta
                );

            undo_move(
                board,
                move,
                &undo
            );

            if (score > best_score)
            {
                best_score =
                    score;

                *best_move =
                    *move;
            }

            if (best_score > alpha)
            {
                alpha =
                    best_score;
            }
        }


        return 1;
    }

    int best_score =
        SEARCH_INF;

    for (int i = 0;
         i < move_count;
         i++)
    {
        const Move *move =
            &ordered_moves[i].move;

        Undo undo;

        make_move(
            board,
            move,
            &undo
        );

        int score =
            minimax(
                board,
                depth - 1,
                alpha,
                beta
            );

        undo_move(
            board,
            move,
            &undo
        );

        if (score < best_score)
        {
            best_score =
                score;

            *best_move =
                *move;
        }

        if (best_score < beta)
        {
            beta =
                best_score;
        }
    }

    return 1;
}