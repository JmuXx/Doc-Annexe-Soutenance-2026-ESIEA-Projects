#include "move/move.h"
#include "test.h"

void test_move(void)
{
    test_suite("Move Tests");

    Move move;

    /*
     * Coup normal
     */
    move = move_create(20, 52, 0, MOVE_NONE);

    ASSERT_INT(20, move.from);
    ASSERT_INT(52, move.to);
    ASSERT_INT(0, move.promotion);
    ASSERT_INT(MOVE_NONE, move.flags);

    ASSERT_FALSE(move_is_capture(&move));
    ASSERT_FALSE(move_is_enpassant(&move));
    ASSERT_FALSE(move_is_castle(&move));
    ASSERT_FALSE(move_is_promotion(&move));

    /*
     * Capture
     */
    move = move_create(10, 26, 0, MOVE_CAPTURE);

    ASSERT_TRUE(move_is_capture(&move));
    ASSERT_FALSE(move_is_promotion(&move));

    /*
     * Promotion
     */
    move = move_create(100, 116, 5, MOVE_PROMOTION);

    ASSERT_TRUE(move_is_promotion(&move));
    ASSERT_FALSE(move_is_capture(&move));

    /*
     * Roque
     */
    move = move_create(4, 6, 0, MOVE_CASTLE);

    ASSERT_TRUE(move_is_castle(&move));

    /*
     * Prise en passant
     */
    move = move_create(68, 83, 0, MOVE_ENPASSANT);

    ASSERT_TRUE(move_is_enpassant(&move));

    /*
     * Capture + Promotion
     */
    move = move_create(
        100,
        117,
        5,
        MOVE_CAPTURE | MOVE_PROMOTION
    );

    ASSERT_TRUE(move_is_capture(&move));
    ASSERT_TRUE(move_is_promotion(&move));
}
