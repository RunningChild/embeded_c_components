#include <stdint.h>
#include <stdbool.h>

#include "calendar.h"
#include "log.h"

#ifndef CALENDAR_TIMER_MS_INTERVAL
#define CALENDAR_TIMER_MS_INTERVAL   (1000)
#endif

static timer_restart_callback_t calendar_timer_restart = NULL;
static calendar_gettime_t calendar_time_get = NULL;

volatile static UTCTime    second_wallclock;      /*This value should be stored in flash*/
volatile static UTCTime    SecondCountRTC;      /*This value should be stored in flash*/
static uint32_t            second_in_hour;
static uint32_t            hour_count;

/*********************************************************************
 * Get month length
 *********************************************************************/
static uint8_t monthLength( uint8_t lpyr, uint8_t mon )
{
    uint8_t days = 31;

    if ( mon == 1 ) // feb
    {
        days = ( 28 + lpyr );
    }
    else
    {
        if ( mon > 6 ) // aug-dec
        {
            mon--;
        }

        if ( mon & 1 )
        {
            days = 30;
        }
    }

    return ( days );
}

/**************************************************************************
* Calculte UTCTime
***************************************************************************/
void ConvertToUTCTime( UTCTimeStruct *tm, UTCTime secTime )
{
    // calculate the time less than a day - hours, minutes, seconds
    {
        uint32_t day = secTime % DAY;
        tm->second = day % 60UL;
        tm->minute = (day % 3600UL) / 60UL;
        tm->hour = day / 3600UL;
    }

    // Fill in the calendar - day, month, year
    {
        uint16_t numDays = secTime / DAY;
        tm->year = BEGYEAR;
        while ( numDays >= YearLength( tm->year ) )
        {
            numDays -= YearLength( tm->year );
            tm->year++;
        }

        tm->month = 0;
        while ( numDays >= monthLength( IsLeapYear( tm->year ), tm->month ))
        {
            numDays -= monthLength( IsLeapYear( tm->year ), tm->month );
            tm->month++;
        }

        tm->day = numDays;
    }
}

/******************************************************************************
 * @brief calendar_set_system_clock
 *****************************************************************************/
void calendar_set_system_clock(time_union_t time, bool need_update)
{
    if(false == need_update)
    {
        //不需要更新系统时间，直接返回
        return;
    }

    uint32_t i = 0;
    UTCTime offset = 0;
    if(time.data == 0){
        SecondCountRTC = offset;
        return;
    }
    //day time
    offset += time.time.second;
    offset += time.time.minute * 60;
    offset += time.time.hour * 60 * 60;

    uint8_t leapYear = IsLeapYear(time.time.year + BEGYEAR);

    offset += DAY * (time.time.day - 1);

    for(i = 0; i < time.time.month - 1; ++i) { //month start from 1
        offset += monthLength(leapYear,i) * DAY;
    }

    for(i = 0; i< time.time.year ;++i) {
        offset += DAY * YearLength(i + BEGYEAR);
    }

    SecondCountRTC = offset;

    /* restart timer */
    calendar_timer_restart(CALENDAR_TIMER_MS_INTERVAL);

    UTCTimeStruct utc_time;
    get_wall_clock_time(&utc_time, false);

    LOG(LEVEL_DEBUG, "set system clock: [%d/%02d/%02d %02d:%02d:%02d]\n",utc_time.year,utc_time.month,utc_time.day,utc_time.hour,utc_time.minute,utc_time.second);
}

/******************************************************************************
 * @brief calendar_get_wall_clock_time
 *****************************************************************************/
void calendar_get_wall_clock_time(UTCTimeStruct * utc_time, bool extern_src)
{
    if (NULL == utc_time)
        return;

    if(false == extern_src)
    {
        /* set a default value */
        utc_time->year = BEGYEAR;
        utc_time->month = 0;
        utc_time->day = 0;
        utc_time->hour = 0;
        utc_time->minute = 0;
        utc_time->second = 0;

        ConvertToUTCTime(utc_time, SecondCountRTC);
        utc_time->month += 1; //calibration
        utc_time->day += 1; //calibration
    }
    else
    {
        //获取系统时间的时候，直接使用外部的RTC芯片
        if(NULL != calendar_time_get)
        {
            calendar_time_get(utc_time);
        }
    }
}

/******************************************************************************
 * @brief is_systme_clock_valid
 *****************************************************************************/
bool is_systme_clock_valid(void)
{
   UTCTimeStruct utc_time;
   get_wall_clock_time(&utc_time, false);
   if(BEGYEAR == utc_time.year)
       return false;
   else
       return true;
}

/******************************************************************************
 * @brief get_day_of_week

    蔡勒（Zeller）公式: 即w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1

    w：星期。
    c：世纪-1。
    y：年（两位数）。
    m：月（m大于等于3，小于等于14。即在蔡勒公式中，某年的1、2月要看作上一年的13、14月来计算，比如2003年1月1日要看作2002年的13月1日来计算）。
    d：日。
    [ ]代表取整，即只要整数部分。

    (C是世纪数减一，y是年份后两位，M是月份，d是日数。1月和2月要按上一年的13月和 14月来算，这时C和y均按上一年取值)

    以2049年10月1日（100周年国庆）为例，过程如下：
    w=y+[y/4]+[c/4]-2c+[26(m+1)/10]+d-1
    =49+[49/4]+[20/4]-2×20+[26×(10+1)/10]+1-1
    =49+[12.25]+5-40+[28.6]
    =49+12+5-40+28
    =54 (除以7余5)
    即2049年10月1日（100周年国庆）是星期5。
 *****************************************************************************/
DAY_OF_WEEK get_day_of_week(UTCTimeStruct utc_time)
{
    int w;
    DAY_OF_WEEK today;
    uint8_t c;

    c = utc_time.year/100;
    utc_time.year -= BEGYEAR;

    if( (1 == utc_time.month)
     || (2 == utc_time.month) )
    {
        utc_time.month += 12;

        utc_time.year--;
        c = utc_time.year/100;
    }

    w = utc_time.year + utc_time.year/4 + c/4 - 2*c +
        26*(utc_time.month + 1)/10 + utc_time.day - 1;

    if(0 >= w)
    {
        w += 1000*LENGTH_OF_WEEK;
    }

    w = w%LENGTH_OF_WEEK;
    today = (DAY_OF_WEEK)w;

    return today;
}

/******************************************************************************
 * @brief get_second_wallclock
 *****************************************************************************/
UTCTime get_second_wallclock(void)
{
   return second_wallclock;
}

/******************************************************************************
 * @brief get_second_counter
 *****************************************************************************/
UTCTime get_second_counter(void)
{
   return SecondCountRTC;
}

/******************************************************************************
 * @brief get_second_in_hour
 *****************************************************************************/
uint32_t get_second_in_hour(void)
{
    return second_in_hour;
}

/******************************************************************************
 * @brief get_hour_count
 *****************************************************************************/
uint32_t get_hour_count(void)
{
    return hour_count;
}

/******************************************************************************
 * @brief timerout handle
 *****************************************************************************/
static void update_wall_clock(void)
{
    /* Here we should use RTC attributes */
    second_wallclock++;
    SecondCountRTC++;
    second_in_hour = SecondCountRTC % 3600;
    hour_count = (SecondCountRTC / 3600 ) % 24;

//    LOG(LEVEL_DEBUG, "hour=%d, second_hour=%d, second_count=%d", hour_count, second_in_hour, SecondCountRTC);
}

/**************************************************************************
 * @brief calendar init
***************************************************************************/
void calendar_init(timer_init_callback_t timer_init, timer_restart_callback_t timer_restart, timer_hander_callback_t *timer_hander, calendar_gettime_t get_time)
{
    /* Init ticks from RTC */
    second_wallclock = 0;
    SecondCountRTC = 0;       /* This should read from flash */

    timer_init();

    calendar_timer_restart = timer_restart;

    *timer_hander = update_wall_clock;

    calendar_time_get = get_time;

    calendar_timer_restart(CALENDAR_TIMER_MS_INTERVAL);

    LOG(LEVEL_DEBUG, "calendar interval=%dms", CALENDAR_TIMER_MS_INTERVAL);
}

