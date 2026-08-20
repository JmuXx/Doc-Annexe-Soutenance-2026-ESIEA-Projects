#ifndef LEGALMOVE_H
#define LEGALMOVE_H

#include "board/board.h"
#include "move/movelist.h"

/*
 * Génère tous les coups légaux pour le joueur au trait.
 *
 * Principe :
 * - génère d'abord les coups pseudo-légaux ;
 * - joue chaque coup temporairement ;
 * - vérifie si le roi du joueur reste en échec ;
 * - annule le coup ;
 * - conserve seulement les coups légaux.
 */
void legalmove_generate(Board *board,
                        MoveList *list);

#endif
