#ifndef PIECE_H
#define PIECE_H

/*
 * Représentation des pièces.
 */

typedef enum
{
    EMPTY = 0,

    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,

    BLACK_PAWN,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING

} Piece;


/*
 * Couleur d'une pièce.
 */
typedef enum
{
    COLOR_WHITE,
    COLOR_BLACK,
    COLOR_NONE

} PieceColor;


/*
 * Conversion pièce <-> caractère.
 */

char piece_to_char(Piece piece);

Piece piece_from_char(char c);


/*
 * Informations sur une pièce.
 */

PieceColor piece_color(Piece piece);

int piece_is_white(Piece piece);

int piece_is_black(Piece piece);

int piece_is_empty(Piece piece);


/*
 * Type de pièce.
 */

int piece_is_pawn(Piece piece);

int piece_is_knight(Piece piece);

int piece_is_bishop(Piece piece);

int piece_is_rook(Piece piece);

int piece_is_queen(Piece piece);

int piece_is_king(Piece piece);

#endif
