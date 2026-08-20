#ifndef ATTACK_H
#define ATTACK_H

#include <stdio.h>

#include "board/board.h"
#include "piece/piece.h"


int board_is_square_attacked(const Board *board,
                             int square,
                             PieceColor attacker);

int board_is_in_check(const Board *board,
                      PieceColor color);

#endif