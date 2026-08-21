#ifndef SEARCH_H
#define SEARCH_H

#include "board/board.h"
#include "move/move.h"

/*
 * Cherche le meilleur coup pour le joueur
 * actuellement au trait.
 *
 * depth :
 * profondeur de recherche.
 *
 * best_move :
 * reçoit le meilleur coup trouvé.
 *
 * Retourne :
 *
 * 1 -> un coup a été trouvé
 * 0 -> aucun coup légal
 */
int search_best_move(Board *board,
                     int depth,
                     Move *best_move);

#endif
