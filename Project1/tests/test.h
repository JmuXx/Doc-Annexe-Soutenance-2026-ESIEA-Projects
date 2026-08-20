#ifndef TEST_H
#define TEST_H


/* ===========================
 *          COLORS
 * =========================== */

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_RESET   "\x1b[0m"



void test_reset(void);

void test_summary(void);

void test_assert_true(int condition,
                      const char *expr,
                      const char *file,
                      int line);

void test_assert_int(int expected,
                     int actual,
                     const char *expectedStr,
                     const char *actualStr,
                     const char *file,
                     int line);

void test_assert_string(const char *expected,
                        const char *actual,
                        const char *expectedStr,
                        const char *actualStr,
                        const char *file,
                        int line);


void test_movegen(void);

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

void test_suite(const char *name);



/* ===========================
 *          MACROS
 * =========================== */

#define ASSERT_TRUE(expr) \
    test_assert_true((expr), #expr, __FILE__, __LINE__)

#define ASSERT_FALSE(expr) \
    test_assert_true(!(expr), "!(" #expr ")", __FILE__, __LINE__)

#define ASSERT_INT(expected, actual) \
    test_assert_int((expected), (actual), \
                    #expected, #actual, \
                    __FILE__, __LINE__)

#define ASSERT_STR(expected, actual) \
    test_assert_string((expected), (actual), \
                       #expected, #actual, \
                       __FILE__, __LINE__)

#endif
