#include "event.h"
#include "dd_port.h"

#include "log.h"

//事件队列句柄集合
static req_event_t   eventlist[REQUEST_EVENT_MAX_NUM];
static uint8_t       event_req_count = 0;

static timer_restart_callback_t event_timer_restart;

/******************************************************************************
 * @brief event_shortest_time_s
 *****************************************************************************/
static void event_shortest_time_s(void)
{
    //找出最短时间的event
    uint16_t min_event_timeout_s = 0;

    for(uint8_t i = 0; i < REQUEST_EVENT_MAX_NUM; i++)
    {
        if(true == eventlist[i].event_used_flag)
        {
            if(0 == min_event_timeout_s)
            {
                min_event_timeout_s = eventlist[i].event_timeout_s;
            }

            if(min_event_timeout_s > eventlist[i].event_timeout_s)
            {
                min_event_timeout_s = eventlist[i].event_timeout_s;
            }
        }
    }

    if(0 != min_event_timeout_s)
    {
        LOG(LEVEL_DEBUG, "min_event_timeout_s=%d", min_event_timeout_s);
        event_timer_restart(min_event_timeout_s*1000);
    }

}

/******************************************************************************
 * @brief event_check_timeout_cb
 *****************************************************************************/
static void event_check_timeout_cb(void)
{
    for(uint8_t i = 0; i < REQUEST_EVENT_MAX_NUM; i++)
    {
        if(true == eventlist[i].event_used_flag)
        {
            if((GET_RUNNING_SECOND() - eventlist[i].event_run_cb_rtc) >= eventlist[i].event_timeout_s)
            {
                //先释放句柄
                event_rsp_cb_t event_cb = eventlist[i].event_cb;
                void * cb_data = eventlist[i].event_cb_data;
                uint32_t cb_data_length = eventlist[i].event_cb_data_length;

                eventlist[i].event_used_flag = false;
                eventlist[i].event_cb = NULL;

                //再执行超时
                if(NULL != event_cb)
                {
                    event_cb(EVENT_RSP_TIMEOUT, cb_data, cb_data_length);
                }

            }
        }
    }

    //检查是否重启事件检测定时器
    event_shortest_time_s();
}

/******************************************************************************
 * @brief event_restart
 * 支持线程安全接口
 *****************************************************************************/
int16_t event_restart(req_event_t *req, const uint16_t timeout_s, const event_rsp_cb_t event_cb, void *cb_data, uint32_t cb_data_length)
{
    APP_ERROR_CHECK_BOOL(0 < timeout_s);

    //再赋值，再分配句柄
    req->event_timeout_s      = timeout_s;
    req->event_cb             = event_cb;
    req->event_cb_data        = cb_data;
    req->event_cb_data_length = cb_data_length ;
    req->event_run_cb_rtc     = GET_RUNNING_SECOND();
    req->event_used_flag      = true;

    //检查是否重启事件检测定时器
    event_shortest_time_s();

    return 0;
}

/******************************************************************************
 * @brief event_operate
 * 支持线程安全接口
 *****************************************************************************/
int16_t event_operate(req_event_t *req)
{
    APP_ERROR_CHECK_BOOL(NULL != req);

    if(true == req->event_used_flag)
    {
        //先释放句柄
        event_rsp_cb_t event_cb = req->event_cb;
        void * cb_data = req->event_cb_data;
        uint32_t cb_data_length = req->event_cb_data_length;

        req->event_used_flag = false;
        req->event_cb = NULL;

        //再执行回调
        if(NULL != event_cb)
        {
            event_cb(EVENT_RSP_SUCCESS, cb_data, cb_data_length);
        }
    }

    return 0;
}

/******************************************************************************
 * @brief event_open
 *****************************************************************************/
int16_t event_open(req_event_t **req)
{
    *req = &eventlist[event_req_count];

    eventlist[event_req_count].event_fd = event_req_count;
    LOG(LEVEL_DEBUG, "[event] fd = %d", event_req_count);

    APP_ERROR_CHECK_BOOL(++event_req_count <= REQUEST_EVENT_MAX_NUM);

    return 0;
}

/*******************************************************************************
 * @brief event_init
 ******************************************************************************/
void event_init(timer_init_callback_t timer_init, timer_restart_callback_t timer_restart, timer_hander_callback_t *timer_hander)
{
    timer_init();

    event_timer_restart = timer_restart;

    *timer_hander = event_check_timeout_cb;
}

