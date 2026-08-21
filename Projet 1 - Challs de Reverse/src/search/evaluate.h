#ifndef EVALUATE_H
#define EVALUATE_H

#include "board/board.h"

/*
 * Evalue matériellement une position.
 *
 * Convention :
 *
 * score > 0 -> avantage Blanc
 * score < 0 -> avantage Noir
 * score = 0 -> matériel équilibré
 */
int evaluate_board(const Board *board);

#endif
