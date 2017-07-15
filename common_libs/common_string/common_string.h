#ifndef __COMMON_STRING_H
#define __COMMON_STRING_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define VERSION(a, b, c, d)             ((uint32_t)(  (a << 24) + (b << 16) + (c << 8) + d ))

#define offset_of(s, m)                  (uint32_t)&(((s *)0)->m)
#define container_of(ptr, type, member)                          \
{                                                                \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);        \
    (type *)( (char *)__mptr - offset_of(type, member) );        \
}


//对外函数接口
void uint16_to_char_array(uint16_t val, uint8_t* data);
void uint32_to_char_array(uint32_t val, uint8_t* data);
int convert_version_string_to_number(char *version_string, uint32_t *version_number);
int convert_number_to_version_string(uint32_t version_number, char *version_string);
bool hexstring_todec(uint8_t *str, uint16_t length);
bool dec_tohexstring(uint8_t * str, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif
