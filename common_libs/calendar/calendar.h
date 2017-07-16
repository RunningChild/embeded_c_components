#ifndef __CALENDAR_H
#define __CALENDAR_H

#include <stdint.h>
#include <stdbool.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IsLeapYear(yr)  (!((yr) % 400) || (((yr) % 100) && !((yr) % 4)))
#define YearLength(yr)  (IsLeapYear(yr) ? 366 : 365)

//UTC started at 00:00:00 January 1, 2000
#define BEGYEAR                     (2000)

//24 hours * 60 minutes * 60 seconds
#define DAY                         (86400UL)

//2000-01-01 is sat
#define SYSTEM_ORIGIN_DAY_OF_WEEK   (Sat)
#define LENGTH_OF_WEEK              (7)

// number of seconds since 0 hrs, 0 minutes, 0 seconds, on the
//1st of January 2000 UTC
//used to store the second counts for RTC
typedef uint32_t UTCTime;

// To be used with
typedef struct
{
    uint16_t year;    // 16 means 2016
    uint8_t month;    // 0-11
    uint8_t day;      // 0-30
    uint8_t second;   // 0-59
    uint8_t minute;   // 0-59
    uint8_t hour;     // 0-23
}
UTCTimeStruct;

typedef enum
{
    Mon  = 0,
    Tue  = 1,
    Wed  = 2,
    Thu  = 3,
    Fri  = 4,
    Sat  = 5,
    Sun  = 6
}DAY_OF_WEEK;

typedef union
{
    uint32_t data;
    UTCTimeStruct time;
} time_union_t;


//函数指针
typedef void (*calendar_gettime_t)(UTCTimeStruct* time);        //由外部实现 获取外部时钟芯片的时间


//对外函数接口
void set_system_clock(time_union_t time, bool need_update);
void get_wall_clock_time(UTCTimeStruct * utc_time, bool extern_src);
bool is_systme_clock_valid (void);
void ConvertToUTCTime( UTCTimeStruct *tm, UTCTime secTime );
DAY_OF_WEEK get_day_of_week(UTCTimeStruct utc_time);
UTCTime get_second_wallclock(void);
UTCTime get_second_counter(void);
uint32_t get_second_in_hour(void);
uint32_t get_hour_count(void);

void calendar_init(timer_init_callback_t timer_init, timer_restart_callback_t timer_restart, timer_hander_callback_t *timer_hander, calendar_gettime_t get_time);

#ifdef __cplusplus
}
#endif

#endif
