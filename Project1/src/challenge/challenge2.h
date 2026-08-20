#ifndef CHALLENGE2_H
#define CHALLENGE2_H

#include <stddef.h>
#include <stdint.h>

#define CHALLENGE2_MOVE_COUNT 8
#define CHALLENGE2_REWARD_KEY_SIZE 32

typedef struct
{
    uint16_t moves[CHALLENGE2_MOVE_COUNT];
    size_t move_count;
    uint32_t seed;

    uint8_t reward_key[CHALLENGE2_REWARD_KEY_SIZE];
    size_t reward_key_size;
    int reward_ready;

} Challenge2State;

void challenge2_init(
    Challenge2State *state
);

void challenge2_record_move(
    Challenge2State *state,
    int from,
    int to
);

int challenge2_is_ready(
    const Challenge2State *state
);

uint32_t challenge2_build_seed(
    const Challenge2State *state
);

int challenge2_check_password(
    Challenge2State *state,
    const char *password
);

void challenge2_print_reward(
    const Challenge2State *state
);

#endif