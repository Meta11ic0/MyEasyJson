#ifndef EASYJSON_H__
#define EASYJSON_H__

#include <stddef.h> /* size_t */

typedef enum{
    EASYJSON_NULL, 
    EASYJSON_FALSE, 
    EASYJSON_TRUE, 
    EASYJSON_NUMBER, 
    EASYJSON_STRING, 
    EASYJSON_ARRAY, 
    EASYJSON_OBJECT 
}easyjson_type;

enum{
    EASYJSON_PARSE_OK = 0,
    EASYJSON_PARSE_EXPECT_VALUE,
    EASYJSON_PARSE_INVALID_VALUE,
    EASYJSON_PARSE_ROOT_NOT_SINGULAR,
    EASYJSON_PARSE_NUMBER_TOO_BIG,
    EASYJSON_PARSE_MISS_QUOTATION_MARK,
    EASYJSON_PARSE_INVALID_STRING_ESCAPE,
    EASYJSON_PARSE_INVALID_STRING_CHAR
};

typedef struct{
    union 
    {
        double n;
        struct 
        { 
            char* s; 
            size_t len; 
        }s;
    };
    easyjson_type type;
}easyjson_value;

#define easyjson_set_null(v) easyjson_free(v)

int easyjson_parse(easyjson_value* v, const char* json);

easyjson_type easyjson_get_type(const easyjson_value* v);

int easyjson_get_boolean(const easyjson_value* v);
void easyjson_set_boolean(easyjson_value* v, int b);

double easyjson_get_number(const easyjson_value* v);
void easyjson_set_number(easyjson_value* v, double n);

const char* easyjson_get_string(const easyjson_value* v);
size_t easyjson_get_string_length(const easyjson_value* v);
void easyjson_set_string(easyjson_value* v, const char* s, size_t len);

#endif