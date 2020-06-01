/**
 * test_common.h - common header file for my code
 *
 * I use several macros to simplify testing, these macros are inspired by gtest,
 * while these macros are not so powerful like gtest, they only provide hint for
 * unexpected expressions.
 *
 */
#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::to_string;
#define TEST_MAIN(statements)                                                  \
    int main(int argc, const char** argv) {                                    \
        statements;                                                            \
        if (!has_warns)                                                        \
            cout << "Tests run finished with 0 warn." << endl;                 \
        return 0;                                                              \
    }
#define PRINT_FAIL_MESSAGE(type, content)                                      \
    cout << type << __FUNCTION__ << "::" << __LINE__ << ": " << content << endl;

#define PRINT_EXPECT_FAIL_MESSAGE(content)                                     \
    {                                                                          \
        PRINT_FAIL_MESSAGE("warn: ", content)                                  \
        has_warns = true;                                                      \
    }

#define EXPECT(expr)                                                           \
    if (!(expr))                                                               \
    PRINT_EXPECT_FAIL_MESSAGE("unexpected value of expression")

#define EXPECT_EQ(a, b)                                                        \
    if ((a) != (b))                                                            \
    PRINT_EXPECT_FAIL_MESSAGE("a(" + to_string(a) + ") is not equal to b(" +   \
                              to_string(b) + ").")

#define EXPECT_LE(a, b)                                                        \
    if ((a) >= (b))                                                            \
    PRINT_EXPECT_FAIL_MESSAGE("a(" + to_string(a) + ") is not less than b(" +  \
                              to_string(b) + ").")

#define EXPECT_GE(a, b)                                                        \
    if ((a) <= (b))                                                            \
    PRINT_EXPECT_FAIL_MESSAGE("a(" + to_string(a) +                            \
                              ") is not greater than b(" + to_string(b) +      \
                              ").")

#define EXPECT_LEQ(a, b)                                                       \
    if ((a) > ((b)))                                                           \
    PRINT_EXPECT_FAIL_MESSAGE("a(" + to_string(a) +                            \
                              ") is not less or equal to b(" + to_string(b) +  \
                              ").")

#define EXPECT_GEQ(a, b)                                                       \
    if ((a) < ((b)))                                                           \
    PRINT_EXPECT_FAIL_MESSAGE("a(" + to_string(a) +                            \
                              ") is not greater or equal to b(" +              \
                              to_string(b) + ").")

bool has_warns = false;