#ifndef __PORT_A_H
#define __PORT_A_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "log.h"

#include "semphr.h"
#include "delay.h"
#include "em_int.h"

#include "platform.h"//add 平台相关

#ifdef __cplusplus
extern "C" {
#endif

#define APP_CHECK_ERROR_HANDLER()           while(1)
#define ENTER_REGION_SECTION()              INT_Disable()
#define EXIT_REGION_SECTION()               INT_Enable()
#define MEM_FREE(p)                         free_overwrite(p)
#define MEM_MALLOC(len)                     malloc_overwrite(len)
#define GET_RUNNING_SECOND()                get_uptime_second()//get_second_wallclock()

#ifdef __cplusplus
}
#endif

#endif

