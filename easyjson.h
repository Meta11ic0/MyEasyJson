#ifndef EASYJSON_H__
#define EASYJSON_H__

#include <stddef.h> /* size_t */

typedef enum
{
    EASYJSON_NULL, 
    EASYJSON_FALSE, 
    EASYJSON_TRUE, 
    EASYJSON_NUMBER, 
    EASYJSON_STRING, 
    EASYJSON_ARRAY, 
    EASYJSON_OBJECT 
}easyjson_value_type;

typedef enum
{
    EASYJSON_PARSE_OK = 0,
    EASYJSON_PARSE_EXPECT_VALUE,
    EASYJSON_PARSE_INVALID_VALUE,
    EASYJSON_PARSE_ROOT_NOT_SINGULAR,
    EASYJSON_PARSE_NUMBER_TOO_BIG,
    EASYJSON_PARSE_MISS_QUOTATION_MARK,
    EASYJSON_PARSE_INVALID_STRING_ESCAPE,
    EASYJSON_PARSE_INVALID_STRING_CHAR
}easyjson_prase_status;

typedef struct
{
    union
    {
        double easyjson_number;
        struct {char * str; size_t size;} easyjson_string;
    };
    easyjson_value_type type;
}easyjson_value;

int easyjson_parse(easyjson_value* v, const char* json);

#endif