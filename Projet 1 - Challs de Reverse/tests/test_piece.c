#include "piece/piece.h"
#include "test.h"

void test_piece(void)
{
    test_suite("Piece Tests");


    ASSERT_INT('P', piece_to_char(WHITE_PAWN));
    ASSERT_INT('N', piece_to_char(WHITE_KNIGHT));
    ASSERT_INT('B', piece_to_char(WHITE_BISHOP));
    ASSERT_INT('R', piece_to_char(WHITE_ROOK));
    ASSERT_INT('Q', piece_to_char(WHITE_QUEEN));
    ASSERT_INT('K', piece_to_char(WHITE_KING));

    ASSERT_INT('p', piece_to_char(BLACK_PAWN));
    ASSERT_INT('n', piece_to_char(BLACK_KNIGHT));
    ASSERT_INT('b', piece_to_char(BLACK_BISHOP));
    ASSERT_INT('r', piece_to_char(BLACK_ROOK));
    ASSERT_INT('q', piece_to_char(BLACK_QUEEN));
    ASSERT_INT('k', piece_to_char(BLACK_KING));

    ASSERT_INT('.', piece_to_char(EMPTY));


    ASSERT_INT(WHITE_PAWN, piece_from_char('P'));
    ASSERT_INT(WHITE_KNIGHT, piece_from_char('N'));
    ASSERT_INT(WHITE_BISHOP, piece_from_char('B'));
    ASSERT_INT(WHITE_ROOK, piece_from_char('R'));
    ASSERT_INT(WHITE_QUEEN, piece_from_char('Q'));
    ASSERT_INT(WHITE_KING, piece_from_char('K'));

    ASSERT_INT(BLACK_PAWN, piece_from_char('p'));
    ASSERT_INT(BLACK_KNIGHT, piece_from_char('n'));
    ASSERT_INT(BLACK_BISHOP, piece_from_char('b'));
    ASSERT_INT(BLACK_ROOK, piece_from_char('r'));
    ASSERT_INT(BLACK_QUEEN, piece_from_char('q'));
    ASSERT_INT(BLACK_KING, piece_from_char('k'));

    ASSERT_INT(EMPTY, piece_from_char('.'));


    ASSERT_TRUE(piece_is_white(WHITE_PAWN));
    ASSERT_TRUE(piece_is_white(WHITE_KING));

    ASSERT_FALSE(piece_is_white(BLACK_PAWN));

    ASSERT_TRUE(piece_is_black(BLACK_QUEEN));
    ASSERT_TRUE(piece_is_black(BLACK_KING));

    ASSERT_FALSE(piece_is_black(WHITE_QUEEN));

    ASSERT_TRUE(piece_is_empty(EMPTY));
    ASSERT_FALSE(piece_is_empty(WHITE_PAWN));


    ASSERT_TRUE(piece_is_pawn(WHITE_PAWN));
    ASSERT_TRUE(piece_is_pawn(BLACK_PAWN));

    ASSERT_TRUE(piece_is_knight(WHITE_KNIGHT));
    ASSERT_TRUE(piece_is_knight(BLACK_KNIGHT));

    ASSERT_TRUE(piece_is_bishop(WHITE_BISHOP));
    ASSERT_TRUE(piece_is_bishop(BLACK_BISHOP));

    ASSERT_TRUE(piece_is_rook(WHITE_ROOK));
    ASSERT_TRUE(piece_is_rook(BLACK_ROOK));

    ASSERT_TRUE(piece_is_queen(WHITE_QUEEN));
    ASSERT_TRUE(piece_is_queen(BLACK_QUEEN));

    ASSERT_TRUE(piece_is_king(WHITE_KING));
    ASSERT_TRUE(piece_is_king(BLACK_KING));
}
