#ifndef EVENT_H
#define EVENT_H

#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include "string.h"
#include "app_error.h"

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif


#define REQUEST_EVENT_MAX_NUM               (5)

#define EVENT_RSP_SUCCESS                   (1000)
#define EVENT_RSP_TIMEOUT                   (1001)


//函数指针
typedef void (*event_rsp_cb_t)(uint16_t err_code, void *cb_data, uint32_t cb_data_length);

typedef struct
{
    bool            event_used_flag;
    uint8_t         event_fd;
    uint16_t        event_timeout_s;
    event_rsp_cb_t  event_cb;
    void *          event_cb_data;
    uint32_t        event_cb_data_length;
    uint32_t        event_run_cb_rtc;
}req_event_t;



//外部函数接口
int16_t event_restart(req_event_t *req, const uint16_t timeout_s, const event_rsp_cb_t event_cb, void *cb_data, uint32_t cb_data_length);
int16_t event_operate(req_event_t *req);
int16_t event_open(req_event_t **req);
void event_init(timer_init_callback_t timer_init, timer_restart_callback_t timer_restart, timer_hander_callback_t *timer_hander);


#ifdef __cplusplus
}
#endif

#endif
