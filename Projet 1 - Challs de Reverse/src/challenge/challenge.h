#ifndef CHALLENGE_H
#define CHALLENGE_H

#include "board/board.h"
#include "move/move.h"

#define CHALLENGE_MOVE_FILE "/tmp/move.dat"
#define CHALLENGE_MAX_PLIES 10

int challenge_reset_move_file(void);

int challenge_write_ai_move(const Move *move);

int challenge_read_ai_move(Board *board,
                           Move *move);

void challenge_print_reward(void);

#endif
