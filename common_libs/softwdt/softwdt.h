#ifndef SOFTWDT_H
#define SOFTWDT_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "app_error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define REQUEST_WDT_MAX_NUM         (20)

typedef struct
{
    uint8_t     wdt_fd;
    uint16_t    wdt_timeout;
    uint8_t     wdt_not_feed_count;
    bool        wdt_mutex_flag;
    bool        wdt_used_flag;
}req_soft_wdt_t;

//函数指针
typedef void (*os_sleep)(uint32_t ms);
typedef void (*softwdt_timeout_handler)(void);


//外部函数接口
int16_t softwdt_open(req_soft_wdt_t **req, uint16_t timeout, uint8_t* thread_name);
void softwdt_feed(req_soft_wdt_t *req);
void softwdt_monitor_thread_util(os_sleep delay, softwdt_timeout_handler handler);

#ifdef __cplusplus
}
#endif

#endif
