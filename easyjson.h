#ifndef EASYJSON_H__
#define EASYJSON_H__

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
    EASYJSON_PARSE_NUMBER_TOO_BIG
};

typedef struct{
    double n;
    easyjson_type type;
}easyjson_value;


int easyjson_parse(easyjson_value* v, const char* json);

easyjson_type easyjson_get_type(const easyjson_value* v);

double easyjson_get_number(const easyjson_value* v);

#endif