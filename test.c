#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "easyjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {\
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")

#define TEST_ERROR(error, json)\
    do {\
        easyjson_value v;\
        v.type = EASYJSON_FALSE;\
        EXPECT_EQ_INT(error, easyjson_parse(&v, json));\
        EXPECT_EQ_INT(EASYJSON_NULL, easyjson_get_type(&v));\
    } while(0)

#define TEST_NUMBER(expect, json)\
    do {\
        easyjson_value v;\
        EXPECT_EQ_INT(EASYJSON_PARSE_OK, easyjson_parse(&v, json));\
        EXPECT_EQ_INT(EASYJSON_NUMBER, easyjson_get_type(&v));\
        EXPECT_EQ_DOUBLE(expect, easyjson_get_number(&v));\
    } while(0)

static void test_parse_null() 
{
    easyjson_value v;
    v.type = EASYJSON_FALSE;
    EXPECT_EQ_INT(EASYJSON_PARSE_OK, easyjson_parse(&v, "null"));
    EXPECT_EQ_INT(EASYJSON_NULL, easyjson_get_type(&v));
}

static void test_parse_true()
{
    easyjson_value v;
    v.type = EASYJSON_NULL;
    EXPECT_EQ_INT(EASYJSON_PARSE_OK, easyjson_parse(&v, "true"));
    EXPECT_EQ_INT(EASYJSON_TRUE, easyjson_get_type(&v));
}

static void test_parse_false()
{
    easyjson_value v;
    v.type = EASYJSON_NULL;
    EXPECT_EQ_INT(EASYJSON_PARSE_OK, easyjson_parse(&v, "false"));
    EXPECT_EQ_INT(EASYJSON_FALSE, easyjson_get_type(&v));
}

static void test_parse_number()
{
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
}

static void test_parse_expect_value()
{
    TEST_ERROR(EASYJSON_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(EASYJSON_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_invalid_value()
{
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "nul");
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "?");
#if 1
    /* invalid number */
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, ".123"); /* at least one digit before '.' */
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "1.");   /* at least one digit after '.' */
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(EASYJSON_PARSE_INVALID_VALUE, "nan");
#endif
}

static void test_parse_root_not_singular()
{
    TEST_ERROR(EASYJSON_PARSE_ROOT_NOT_SINGULAR, "null x");
#if 0
    /* invalid number */
    TEST_ERROR(EASYJSON_PARSE_ROOT_NOT_SINGULAR, "0123"); /* after zero should be '.' , 'E' , 'e' or nothing */
    TEST_ERROR(EASYJSON_PARSE_ROOT_NOT_SINGULAR, "0x0");
    TEST_ERROR(EASYJSON_PARSE_ROOT_NOT_SINGULAR, "0x123");
#endif
}

static void test_parse_number_too_big() {
#if 0
    TEST_ERROR(EASYJSON_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(EASYJSON_PARSE_NUMBER_TOO_BIG, "-1e309");
#endif
}

static void test_parse()
{
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_number();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}


int main()
{
    test_parse();
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}