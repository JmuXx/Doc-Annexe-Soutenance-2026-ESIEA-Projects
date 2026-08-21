#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "board/board.h"
#include "move/movelist.h"

/*
 * Génère les coups pseudo-légaux pour la position.
 *
 * Les coups générés respectent les mouvements des pièces
 * et les règles de déplacement de base, mais ne vérifient
 * pas encore si le roi du joueur est en échec.
 */
void movegen_generate(const Board *board, MoveList *list);

#endif
