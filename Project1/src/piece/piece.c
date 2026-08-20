#include "piece.h"

char piece_to_char(Piece piece)
{
    switch(piece)
    {
        case WHITE_PAWN:   return 'P';
        case WHITE_KNIGHT: return 'N';
        case WHITE_BISHOP: return 'B';
        case WHITE_ROOK:   return 'R';
        case WHITE_QUEEN:  return 'Q';
        case WHITE_KING:   return 'K';

        case BLACK_PAWN:   return 'p';
        case BLACK_KNIGHT: return 'n';
        case BLACK_BISHOP: return 'b';
        case BLACK_ROOK:   return 'r';
        case BLACK_QUEEN:  return 'q';
        case BLACK_KING:   return 'k';

        default:
            return '.';
    }
}

Piece piece_from_char(char c)
{
    switch(c)
    {
        case 'P': return WHITE_PAWN;
        case 'N': return WHITE_KNIGHT;
        case 'B': return WHITE_BISHOP;
        case 'R': return WHITE_ROOK;
        case 'Q': return WHITE_QUEEN;
        case 'K': return WHITE_KING;

        case 'p': return BLACK_PAWN;
        case 'n': return BLACK_KNIGHT;
        case 'b': return BLACK_BISHOP;
        case 'r': return BLACK_ROOK;
        case 'q': return BLACK_QUEEN;
        case 'k': return BLACK_KING;

        default:
            return EMPTY;
    }
}

PieceColor piece_color(Piece piece)
{
    if(piece >= WHITE_PAWN && piece <= WHITE_KING)
        return COLOR_WHITE;

    if(piece >= BLACK_PAWN && piece <= BLACK_KING)
        return COLOR_BLACK;

    return COLOR_NONE;
}

int piece_is_white(Piece piece)
{
    return piece_color(piece) == COLOR_WHITE;
}

int piece_is_black(Piece piece)
{
    return piece_color(piece) == COLOR_BLACK;
}

int piece_is_empty(Piece piece)
{
    return piece == EMPTY;
}

int piece_is_pawn(Piece piece)
{
    return piece == WHITE_PAWN || piece == BLACK_PAWN;
}

int piece_is_knight(Piece piece)
{
    return piece == WHITE_KNIGHT || piece == BLACK_KNIGHT;
}

int piece_is_bishop(Piece piece)
{
    return piece == WHITE_BISHOP || piece == BLACK_BISHOP;
}

int piece_is_rook(Piece piece)
{
    return piece == WHITE_ROOK || piece == BLACK_ROOK;
}

int piece_is_queen(Piece piece)
{
    return piece == WHITE_QUEEN || piece == BLACK_QUEEN;
}

int piece_is_king(Piece piece)
{
    return piece == WHITE_KING || piece == BLACK_KING;
}
