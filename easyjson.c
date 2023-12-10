#include "easyjson.h"

//解析器，用于暂存传进来的字符串
typedef struct 
{
    const char * value;
}easyjson_praser;


//去除字符串首部空白字符
void easyjson_parse_whitespace(easyjson_praser * praser)
{
    const char *p = praser->value;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    praser->value = p;
}

//解析常量字符串
int easyjson_parse_literal(easyjson_praser * praser, easyjson_value * json, const char * literal, easyjson_value_type literal_type)
{
    size_t i = 1;
    //中间的循环条件是指当literal[i + 1] = '\0'的时候，跳出循环
    for (; literal[i]; i++)
        if (praser->value[i] != literal[i])
            return EASYJSON_PARSE_INVALID_VALUE;
    praser->value += i + 1;
    json->type = literal_type;
    return EASYJSON_PARSE_OK;
}


//根据字符串首字符进行分流
int easyjson_parse_value(easyjson_praser * praser, easyjson_value * value)
{
    int ret;
    switch (*praser->value)
    {
        case 't' : return easyjson_parse_literal(praser, value, "true", EASYJSON_TRUE);
        case 'f' : return easyjson_parse_literal(praser, value, "false", EASYJSON_FALSE);
        case 'n' : return easyjson_parse_literal(praser, value, "null", EASYJSON_NULL);
    default:
        break;
    }
    return ret;
}

//解析函数入口
int easyjson_parse(easyjson_value* json, const char* cstylejsonstr)
{
    int ret;
    easyjson_praser praser;
    praser.value = cstylejsonstr;
    json->type = EASYJSON_NULL;
    easyjson_parse_whitespace(&praser);
    if((ret = easyjson_parse_value(&praser, json)) == EASYJSON_PARSE_OK)
    {
        easyjson_parse_whitespace(&praser);
        if(*praser.value != '\0')
        {
            json->type = EASYJSON_NULL;
            ret = EASYJSON_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

