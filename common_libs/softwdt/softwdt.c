#include "softwdt.h"

#include "log.h"

//软件看门狗句柄集合
static req_soft_wdt_t   softwdt[REQUEST_WDT_MAX_NUM];
static uint8_t          softwdt_req_count = 0;

/******************************************************************************
 * @brief softwdt_open
 * 该函数不支持线程安全，必须在任务调度之前，调用
 *****************************************************************************/
int16_t softwdt_open(req_soft_wdt_t **req, uint16_t timeout, uint8_t* thread_name)
{
    *req = &softwdt[softwdt_req_count];

    softwdt[softwdt_req_count].wdt_fd               = softwdt_req_count;
    softwdt[softwdt_req_count].wdt_timeout          = timeout;
    softwdt[softwdt_req_count].wdt_not_feed_count   = 0;
    softwdt[softwdt_req_count].wdt_mutex_flag       = false;
    softwdt[softwdt_req_count].wdt_used_flag        = true;

    LOG(LEVEL_DEBUG, "[softwdt] fd = %d, timeout = %d [%s]", softwdt_req_count, timeout, thread_name);

    APP_ERROR_CHECK_BOOL(++softwdt_req_count <= REQUEST_WDT_MAX_NUM);

    return 0;
}

/******************************************************************************
 * @brief softwdt_open
 *****************************************************************************/
void softwdt_feed(req_soft_wdt_t *req)
{
    if (req != NULL)
    {
        req->wdt_mutex_flag = true;
        req->wdt_not_feed_count = 0;
        req->wdt_mutex_flag = false;
    }
}

/******************************************************************************
 * @brief softwdt_monitor_thread_util
 *****************************************************************************/
void softwdt_monitor_thread_util(os_sleep delay, softwdt_timeout_handler handler)
{
    uint8_t i;
//    LOG(LEVEL_DEBUG, "softwdt_monitor_thread_util");

    for (i = 0; i < REQUEST_WDT_MAX_NUM; i++)
    {
        if (true == softwdt[i].wdt_used_flag)
        {
            if (softwdt[i].wdt_not_feed_count > softwdt[i].wdt_timeout)
            {
                softwdt[i].wdt_not_feed_count = 0;
                LOG(LEVEL_FORCE, "[softwdt] fd = %d, monitor timeout", softwdt[i].wdt_fd);
                if(NULL != handler)
                {
                    handler();//软件看门狗超时处理
                }
            }

            if(false == softwdt[i].wdt_mutex_flag)
            {
                softwdt[i].wdt_not_feed_count++;//监控线程每次调用都++
            }
        }
    }

    delay(1000);//睡眠1秒
}

