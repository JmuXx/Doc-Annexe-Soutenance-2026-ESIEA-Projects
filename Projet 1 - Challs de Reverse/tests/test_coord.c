#include "coord/coord.h"
#include "test.h"

void test_coord(void)
{
    test_suite("Coordinate Tests");

    char coord[3];

    ASSERT_INT(20, coord_from_string("e2"));
    ASSERT_INT(52, coord_from_string("e4"));
    ASSERT_INT(0, coord_from_string("a1"));
    ASSERT_INT(119, coord_from_string("h8"));

    ASSERT_INT(-1, coord_from_string("z9"));
    ASSERT_INT(-1, coord_from_string("i4"));
    ASSERT_INT(-1, coord_from_string("a9"));

    coord_to_string(20, coord);
    ASSERT_STR("e2", coord);

    coord_to_string(119, coord);
    ASSERT_STR("h8", coord);

    ASSERT_TRUE(coord_is_valid(20));
    ASSERT_TRUE(coord_is_valid(119));

    ASSERT_FALSE(coord_is_valid(120));
    ASSERT_FALSE(coord_is_valid(128));
}
