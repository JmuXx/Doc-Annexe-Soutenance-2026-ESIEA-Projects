#ifndef MOVELIST_H
#define MOVELIST_H

#include "defs.h"
#include "move.h"

#define MAX_MOVES 256

typedef struct
{
    Move moves[MAX_MOVES];

    int count;

} MoveList;


void movelist_clear(MoveList *list);


void movelist_add(MoveList *list,
                  Move move);


int movelist_count(const MoveList *list);


const Move *movelist_get(const MoveList *list,
                   int index);


int movelist_find(const MoveList *list,
                  Move move);

#endif
