#ifndef DATETIME_H
#define DATETIME_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "app_error.h"

#ifdef __cplusplus
extern "C" {
#endif


#define BEIJING_TIME_ZONE           (8)
#define TIMESTAMP_STRING_SIZE       (14)

typedef struct
{
    uint8_t timestamp[TIMESTAMP_STRING_SIZE];
}st_timestamp;


typedef enum
{
    DATE_EFFECT = 0,              //有效
    DATE_INEFFECT = 1,            //未生效
    DATE_OVERDUE = 2,             //过期
    DATE_CHECK_ERROR = 3,         //错误
}check_date_valid_t;


//对外函数接口
uint32_t linux_mktime(uint16_t year0, uint16_t mon0,
                           uint16_t day, uint16_t hour,
                           uint16_t min, uint16_t sec,
                           int time_zone);
uint32_t get_curr_timestamp_int(bool extern_src);
check_date_valid_t check_time_valid(const uint8_t start_time[6], const uint8_t stop_time[6], bool extern_src);
uint32_t get_timestamp_diff(uint32_t timestamp1, uint32_t timestamp2);
bool check_time_format(const uint8_t start_time[6], const uint8_t stop_time[6]);

#ifdef __cplusplus
}
#endif

#endif
