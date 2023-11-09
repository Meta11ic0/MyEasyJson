#include "easyjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */
#include <string.h>

#define EXPECT(c, ch) do { assert(*c->json == (ch)); c->json++; } while(0)

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
    //开头不允许“+”，和“.”
    if(c->json[0] == '+' || c->json[0] == '.')
        return EASYJSON_PARSE_INVALID_VALUE;
    
    char* end = NULL;
    /* \TODO validate number */
    v->n = strtod(c->json, &end);
    //取结尾字符
    char last_char = c->json[strlen(c->json)-1];
    //结尾字符不能为"."
    if (last_char == '.')
        return EASYJSON_PARSE_INVALID_VALUE;

    c->json = end;
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
            ret = EASYJSON_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}

