#include "test.h"

void test_coord(void);
void test_move(void);
void test_piece(void);
void test_movelist(void);
void test_movegen(void);
void test_bishop_movegen(void);
void test_rook_movegen(void);
void test_queen_movegen(void);
void test_king_movegen(void);
void test_attack(void);
void test_board(void);
void test_makemove(void);
void test_legalmove(void);
void test_promotion(void);
void test_enpassant(void);
void test_castling(void);
void test_evaluate(void);
void test_search(void);

int main(void)
{
    test_reset();

    test_coord();

    test_move();

    test_piece();

    test_bishop_movegen();

    test_rook_movegen();

    test_queen_movegen();

    test_king_movegen();

    test_attack();

    test_board();

    test_makemove();

    test_legalmove();

    test_promotion();

    test_enpassant();

    test_castling();

    test_movelist();

    test_movegen();

    test_evaluate();

    test_search();

    test_summary();

    return 0;
}
