#include "move/move.h"
#include "move/movelist.h"
#include "test.h"

void test_movelist(void)
{
    test_suite("MoveList Tests");

    MoveList list;

    const Move move1 = move_create(20, 36, PROMOTION_NONE, MOVE_NONE);
    const Move move2 = move_create(21, 37, PROMOTION_NONE, MOVE_CAPTURE);
    const Move move3 = move_create(
        22,
        38,
        PROMOTION_QUEEN,
        MOVE_PROMOTION
    );
    
    movelist_clear(&list);

    ASSERT_INT(0, movelist_count(&list));

    movelist_add(&list, move1);

    ASSERT_INT(1, movelist_count(&list));

    movelist_add(&list, move2);

    ASSERT_INT(2, movelist_count(&list));

    movelist_add(&list, move3);

    ASSERT_INT(3, movelist_count(&list));


    const Move *m;

    m = movelist_get(&list, 0);

    ASSERT_INT(move1.from, m->from);
    ASSERT_INT(move1.to, m->to);
    ASSERT_INT(move1.flags, m->flags);

    m = movelist_get(&list, 1);

    ASSERT_INT(move2.from, m->from);
    ASSERT_INT(move2.to, m->to);
    ASSERT_INT(move2.flags, m->flags);

    m = movelist_get(&list, 2);

    ASSERT_INT(move3.from, m->from);
    ASSERT_INT(move3.to, m->to);
    ASSERT_INT(move3.promotion, m->promotion);
    ASSERT_INT(move3.flags, m->flags);


    ASSERT_INT(0, movelist_find(&list, move1));
    ASSERT_INT(1, movelist_find(&list, move2));
    ASSERT_INT(2, movelist_find(&list, move3));

    const Move unknown = move_create(50, 66, PROMOTION_NONE, MOVE_NONE);

    ASSERT_INT(-1, movelist_find(&list, unknown));

    movelist_clear(&list);

    ASSERT_INT(0, movelist_count(&list));

    ASSERT_INT(-1, movelist_find(&list, move1));
}
