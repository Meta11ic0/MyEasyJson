#include "easyjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL strtod() */
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */


#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define EXPECTDIGITSTR(p) \
    do\
    {\
        if (!ISDIGIT(*p)) \
            return EASYJSON_PARSE_INVALID_VALUE;\
        for (p++; ISDIGIT(*p); p++);\
    }\
    while(0);
#define EXPECTZEROORNOTZEROSTARTEDDIGITSTR(p) \
    do\
    {\
        if(*p == '0')\
        {\
            p++;\
        }\
        else\
        {\
            EXPECTDIGITSTR(p);\
        }\
    }\
    while(0);


typedef struct{
    const char * json;
}easyjson_context;

static int easyjson_parse_literal(easyjson_context* c, easyjson_value* v) 
{
    if(c->json[0] == 'n' && c->json[1] == 'u' && c->json[2] == 'l' && c->json[3] == 'l')
    {
        c->json += 4;
        v->type = EASYJSON_NULL;
    }
    else if(c->json[0] == 't' && c->json[1] == 'r' && c->json[2] == 'u' && c->json[3] == 'e')
    {
        c->json += 4;
        v->type = EASYJSON_TRUE;
    }
    else if (c->json[0] == 'f' && c->json[1] == 'a' && c->json[2] == 'l' && c->json[3] == 's' && c->json[4] == 'e')
    {
        c->json += 5;
        v->type = EASYJSON_FALSE;
    }
    else
        return EASYJSON_PARSE_INVALID_VALUE;
    return EASYJSON_PARSE_OK;
}

static int easyjson_parse_number(easyjson_context* c, easyjson_value* v) 
{   
    //p指针用于检测传进来的字符串
    const char* p = c->json;
    //不能是'+'号
    if(*p == '-')
        p++;
    
    //要么是'0'， 要么是非0开头的数字串
    if(*p == '0')
    {
        p++;
    }
    else
    {
        if (!ISDIGIT1TO9(*p)) 
            return EASYJSON_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    //小数
    if(*p == '.')
    {
        p++;
        if (!ISDIGIT(*p)) 
            return EASYJSON_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    //指数
    if(*p == 'E' || *p == 'e')
    {
        p++;
        if(*p == '+' || *p == '-')
            p++;
        if (!ISDIGIT(*p)) 
            return EASYJSON_PARSE_INVALID_VALUE;
        for (p++; ISDIGIT(*p); p++);
    }
    errno = 0;
    v->n = strtod(c->json, NULL);
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return EASYJSON_PARSE_NUMBER_TOO_BIG;
    c->json = p;
    v->type = EASYJSON_NUMBER;
    return EASYJSON_PARSE_OK;
}

static void easyjson_parse_whitespace(easyjson_context* c) 
{
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int easyjson_parse_value(easyjson_context* c, easyjson_value* v) 
{
    switch (*c->json) 
    {
        case 'n':case 't':case 'f':  return easyjson_parse_literal(c, v);
        case '\0': return EASYJSON_PARSE_EXPECT_VALUE;
        default:   return easyjson_parse_number(c, v);
    }
}

easyjson_type easyjson_get_type(const easyjson_value* v)
{    
    assert(v != NULL);
    return v->type;
}

double easyjson_get_number(const easyjson_value* v)
{
    assert(v != NULL && v->type == EASYJSON_NUMBER);
    return v->n;
}

int easyjson_parse(easyjson_value* v, const char* json)
{
    easyjson_context c;
    int ret = EASYJSON_PARSE_INVALID_VALUE;
    assert(v != NULL);
    c.json = json;
    v->type = EASYJSON_NULL;
    easyjson_parse_whitespace(&c);
    if((ret = easyjson_parse_value(&c, v)) == EASYJSON_PARSE_OK)
    {
        easyjson_parse_whitespace(&c);
        if(*c.json != '\0')
        {
            v->type = EASYJSON_NULL;
            ret = EASYJSON_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

