#include <stdio.h>
#include <string.h>

#include "test.h"

static int testsRun = 0;
static int testsPassed = 0;
static int testsFailed = 0;

void test_reset(void)
{
    testsRun = 0;
    testsPassed = 0;
    testsFailed = 0;
}

void test_assert_true(int condition,
                      const char *expr,
                      const char *file,
                      int line)
{
    testsRun++;

    if(condition)
    {
        testsPassed++;
        printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s\n", expr);
    }
    else
    {
        testsFailed++;

        printf(COLOR_RED "[FAIL]" COLOR_RESET " %s\n", expr);
        printf("       File : %s\n", file);
        printf("       Line : %d\n", line);
    }
}

void test_assert_int(int expected,
                     int actual,
                     const char *expectedStr,
                     const char *actualStr,
                     const char *file,
                     int line)
{
    testsRun++;

    if(expected == actual)
    {
        testsPassed++;

        printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s == %s\n", expectedStr, actualStr);
    }
    else
    {
        testsFailed++;

        printf(COLOR_RED "[FAIL]" COLOR_RESET " %s == %s\n", expectedStr, actualStr);
        printf("       Expected : %d\n", expected);
        printf("       Actual   : %d\n", actual);
        printf("       File     : %s\n", file);
        printf("       Line     : %d\n", line);
    }
}

void test_assert_string(const char *expected,
                        const char *actual,
                        const char *expectedStr,
                        const char *actualStr,
                        const char *file,
                        int line)
{
    testsRun++;

    if(strcmp(expected, actual) == 0)
    {
        testsPassed++;

        printf(COLOR_GREEN "[PASS]" COLOR_RESET " %s == %s\n", expectedStr, actualStr);
    }
    else
    {
        testsFailed++;

        printf(COLOR_RED "[FAIL]" COLOR_RESET " %s == %s\n", expectedStr, actualStr);

        printf("       Expected : \"%s\"\n", expected);
        printf("       Actual   : \"%s\"\n", actual);
        printf("       File     : %s\n", file);
        printf("       Line     : %d\n", line);
    }
}

void test_suite(const char *name)
{
    printf("\n");

    printf(COLOR_BLUE);
    printf("========================================\n");
    printf(" %s\n", name);
    printf("========================================\n");
    printf(COLOR_RESET);
}

void test_summary(void)
{
    printf("\n");
    printf("========================================\n");
    printf("Tests run    : %d\n", testsRun);

    printf(COLOR_GREEN "Tests passed : %d\n" COLOR_RESET,
           testsPassed);

    if(testsFailed == 0)
    {
        printf(COLOR_GREEN "Tests failed : %d\n" COLOR_RESET,
               testsFailed);
    }
    else
    {
        printf(COLOR_RED "Tests failed : %d\n" COLOR_RESET,
               testsFailed);
    }

    printf("========================================\n");
}
