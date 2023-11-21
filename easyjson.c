#include "easyjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL strtod() */
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */


#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
#define PUTC(c, ch)         do { *(char*)easyjson_context_push(c, sizeof(char)) = (ch); } while(0)

#define easyjson_init(v)    do { (v)->type = EASYJSON_NULL; } while(0)
#define easyjson_set_null(v) easyjson_free(v)

#ifndef EASYJSON_PARSE_STACK_INIT_SIZE
#define EASYJSON_PARSE_STACK_INIT_SIZE 256
#endif

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
    char* stack;
    size_t size, top;
}easyjson_context;

static void* easyjson_context_push(easyjson_context* c, size_t size) 
{
    void* ret;
    assert(size > 0);
    if (c->top + size >= c->size) {
        if (c->size == 0)
            c->size = EASYJSON_PARSE_STACK_INIT_SIZE;
        while (c->top + size >= c->size)
            c->size += c->size >> 1;  /* c->size * 1.5 */
        c->stack = (char*)realloc(c->stack, c->size);
    }
    ret = c->stack + c->top;
    c->top += size;
    return ret;
}

static void* easyjson_context_pop(easyjson_context* c, size_t size) 
{
    assert(c->top >= size);
    return c->stack + (c->top -= size);
}

void easyjson_free(easyjson_value* v) 
{
    assert(v != NULL);
    if (v->type == EASYJSON_STRING)
        free(v->s.s);
    v->type = EASYJSON_NULL;
}

void easyjson_set_string(easyjson_value* v, const char* s, size_t len)
{
    assert(v != NULL && (s != NULL || len == 0));
    easyjson_free(v);
    v->s.s = (char*)malloc(len + 1);
    memcpy(v->s.s, s, len);
    v->s.s[len] = '\0';
    v->s.len = len;
    v->type = EASYJSON_STRING;
}

static int easyjson_parse_string(easyjson_context* c, easyjson_value* v) 
{
    size_t head = c->top, len;
    const char* p;
    EXPECT(c, '\"');
    p = c->json;
    for (;;) 
    {
        char ch = *p++;
        switch (ch) 
        {
            case '\"':
                len = c->top - head;
                easyjson_set_string(v, (const char*)easyjson_context_pop(c, len), len);
                c->json = p;
                return EASYJSON_PARSE_OK;
            case '\0':
                c->top = head;
                return EASYJSON_PARSE_MISS_QUOTATION_MARK;
            default:
                PUTC(c, ch);
        }
    }
}

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

easyjson_type easyjson_get_type(const easyjson_value* v)
{
    assert(v != NULL);
    return v->type;
}

int easyjson_get_boolean(const easyjson_value* v)
{
    /* \TODO */
    return 0;
}

void easyjson_set_boolean(easyjson_value* v, int b)
{
    /* \TODO */
}

double easyjson_get_number(const easyjson_value* v)
{
    assert(v != NULL && v->type == EASYJSON_NUMBER);
    return v->n;
}

void easyjson_set_number(easyjson_value* v, double n)
{
    /* \TODO */
}

const char* easyjson_get_string(const easyjson_value* v) 
{
    assert(v != NULL && v->type == EASYJSON_STRING);
    return v->s.s;
}

size_t easyjson_get_string_length(const easyjson_value* v) 
{
    assert(v != NULL && v->type == EASYJSON_STRING);
    return v->s.len;
}

void easyjson_set_string(easyjson_value* v, const char* s, size_t len)
{
    assert(v != NULL && (s != NULL || len == 0));
    easyjson_free(v);
    v->s.s = (char*)malloc(len + 1);
    memcpy(v->s.s, s, len);
    v->s.s[len] = '\0';
    v->s.len = len;
    v->type = EASYJSON_STRING;
}

static int easyjson_parse_value(easyjson_context* c, easyjson_value* v) 
{
    switch (*c->json) 
    {
        case 'n':case 't':case 'f':  return easyjson_parse_literal(c, v);
        case '"' :easyjson_parse_string(c, v);
        case '\0': return EASYJSON_PARSE_EXPECT_VALUE;
        default:   return easyjson_parse_number(c, v);
    }
}

int easyjson_parse(easyjson_value* v, const char* json)
{
    easyjson_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    c.stack = NULL;        /* <- */
    c.size = c.top = 0;    /* <- */
    easyjson_init(v);
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
    assert(c.top == 0);    /* <- */
    free(c.stack);         /* <- */
    return ret;
}

